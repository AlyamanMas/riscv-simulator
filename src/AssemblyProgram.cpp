//
// Created by yaman on 17/11/23.
//

#include <sstream>
#include <iostream>
#include <string>
#include "AssemblyProgram.h"

using namespace std;

void AssemblyProgram::set_register(RegIndex_t index, RegValue_t value) {
    if (index < 0 || index >= 32) {
        throw std::out_of_range("Register index out of range");
    }
    if (index == 0) {
        return;
    }
    register_file[index] = value;
}

RegValue_t AssemblyProgram::get_register(RegIndex_t index) const {
    if (index < 0 || index >= 32) {
        throw std::out_of_range("Register index out of range");
    }
    return register_file[index];
}

AssemblyProgram::AssemblyProgram(std::string program_text, uint32_t starting_address)
        : pc(starting_address), memory(Memory()), top_instruction_address(starting_address), register_file({}) {
    stringstream program_stream(program_text);
    uint32_t current_address = 0;
    for (string line; getline(program_stream, line);) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        Instruction32::ParsingException_t error;
        std::optional<Instruction32::UnresolvedLabel_t> unresolved_label;
        auto instruction = Instruction32(line, error, unresolved_label);

        if (error.error.value_or("").find("has too few operands"s) == string::npos
            && error.error.value_or("").find("only has a label"s) == string::npos
            && error.error.value_or("").find("empty line"s) == string::npos) {

            instructions.push_back({current_address, instruction});
            current_address += 4;
        }
        if (unresolved_label.has_value()) {
            label_map[unresolved_label.value()] = current_address;
        }

        if (error.error.has_value() && !error.is_warning) {
            cerr << "Error: " << error.error.value() << endl;
            throw logic_error(error.error.value());
        } else if (error.error.has_value() && error.is_warning) {
            if (error.error.value().find("only has a label") == string::npos)
                cerr << "Warning: " << error.error.value() << endl;
        }
    }

    resolve_labels();
}

void AssemblyProgram::resolve_labels() {
    for (auto &instruction : instructions) {
        for (auto &operand : instruction.instruction.operands) {
            if (holds_alternative<Instruction32::UnresolvedLabel_t>(operand)) {
                auto label = get<Instruction32::UnresolvedLabel_t>(operand);
                if (label_map.find(label) == label_map.end()) {
                    throw logic_error("Label " + label + " not found");
                }
                // calculate offset from this instruction's address to the label's address
                int offset = ((int32_t)(label_map[label] - instruction.address)) / 2;
                // convert offset to a signed 12-bit immediate
                auto immediate = static_cast<Instruction32::Immediate_t>(offset);
                operand = immediate;
            }
        }
    }

    // to save some memory space, we can clear the label map now that we're done with it
    // disabled for debugging purposes
//    label_map.clear();
}

bool AssemblyProgram::execute_next_instruction() {
    uint32_t old_pc = pc;
    if (pc > instructions.back().address) {
        reached_end_of_program = true;
        return true;
    }

    auto instruction = instructions[pc / 4];
    instruction.instruction.execute(
            [this](RegIndex_t index, RegValue_t value) { this->set_register(index, value); },
            [this](RegIndex_t index) { return this->get_register(index); },
            memory, pc);

    if (pc == old_pc) {
        pc += 4;
    }

    return false;
}

void AssemblyProgram::run() {
    while (!reached_end_of_program) {
        execute_next_instruction();
    }
}