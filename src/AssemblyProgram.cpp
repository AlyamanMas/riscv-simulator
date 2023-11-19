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
            && error.error.value_or("").find("only has a label"s) == string::npos) {

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
}
