//
// Created by yaman on 13/11/23.
//

#include "Instruction32.h"

#include <algorithm>
#include <vector>
#include <optional>
#include <functional>

#include "RV32I_Instruction.h"

using namespace std;

namespace parsing {
    // note reg_text must not have a leading or trailing comma or space,
    // it should exactly be in the format 'xi' where i is a number between 0 and 31
    // or a register convention name
    optional<RegIndex_t> parse_regindex(const string &reg_text) {
        if (reg_text.at(0) == 'x') {
            if (reg_text.size() == 2 && isdigit(reg_text.at(1))) {
                return {reg_text.at(1) - '0'};
            } else if (reg_text.size() == 3 && isdigit(reg_text.at(1)) && isdigit(reg_text.at(2))) {
                if (stoi(reg_text.substr(1)) > 31) {
                    return nullopt;
                }
                return {(reg_text.at(1) - '0') * 10 + (reg_text.at(2) - '0')};
            }
        } else if (register_convention_map.contains(reg_text)) {
            return {register_convention_map.at(reg_text)};
        }
        return nullopt;
    }

    optional<Instruction32::Immediate_t>
    parse_immediate(const string &imm_text, bool &gte_modulo, int modulo = 0x1000) {
        gte_modulo = false;
        if (imm_text.at(0) == '0' && imm_text.at(1) == 'x') { // hex
            auto imm = stoi(imm_text, nullptr, 16);
            if (imm >= modulo) {
                gte_modulo = true;
            }
            return {imm & modulo};
        } else if (imm_text.at(0) == '0' && imm_text.at(1) == 'b') { // binary
            auto imm = stoi(imm_text, nullptr, 2);
            if (imm >= modulo) {
                gte_modulo = true;
            }
            return {imm & modulo};
        } else if (isdigit(imm_text.at(0)) || imm_text.at(0) == '-') { // decimal
            auto imm = stoi(imm_text);
            if (imm >= modulo) {
                gte_modulo = true;
            }
            return {imm & modulo};
        }
        return nullopt;
    }

    optional<tuple<Instruction32::Immediate_t, RegIndex_t>> parse_immediate_with_offset(
            const string &imm_text,
            Instruction32::ParsingException_t &exception
    ) {
        if (imm_text.find('(') == string::npos || imm_text.find(')') == string::npos) {
            exception = {
                    logic_error("The instruction \""s + imm_text +
                                "\" has an invalid immediate value: " + imm_text),
                    false
            };
            return nullopt;
        }
        auto imm = parse_immediate(imm_text.substr(0, imm_text.find('(')), exception.is_warning);
        if (!imm.has_value()) {
            exception = {
                    logic_error("The instruction \""s + imm_text +
                                "\" has an invalid immediate value: " + imm_text),
                    false
            };
            return nullopt;
        }
        auto reg_index = parse_regindex(imm_text.substr(imm_text.find('(') + 1, imm_text.find(')') - 1));
        if (!reg_index.has_value()) {
            exception = {
                    logic_error("The instruction \""s + imm_text +
                                "\" has an invalid register name: " + imm_text),
                    false
            };
            return nullopt;
        }
        return {make_tuple(imm.value(), reg_index.value())};
    }

    void parse_operands(
            Instruction32 &instruction,
            vector<string> &words,
            bool has_label,
            Instruction32::ParsingException_t &exception,
            const string &instruction_text
    ) {
        switch (get_instruction_format(instruction.type)) {
            case R_TYPE: {
                for (int i = 0; i < 3; ++i) {
                    auto reg_index =
                            parse_regindex(words.at(has_label + i + 1));
                    if (!reg_index.has_value()) {
                        exception = {
                                logic_error("The instruction \""s + instruction_text +
                                            "\" has an invalid register name: " + words.at(has_label + i + 1)),
                                false
                        };
                        return;
                    }
                    instruction.operands[i] = reg_index.value();
                }
                break;
            }
            case I_TYPE: {
                for (int i = 0; i < 2; ++i) {
                    auto reg_index =
                            parse_regindex(words.at(has_label + i + 1));
                    if (!reg_index.has_value()) {
                        exception = {
                                logic_error("The instruction \""s + instruction_text +
                                            "\" has an invalid register name: " + words.at(has_label + i + 1)),
                                false
                        };
                        return;
                    }
                    instruction.operands[i] = reg_index.value();
                }
                bool greater_than_12_bits;
                auto imm =
                        parse_immediate(words.at(has_label + 3), greater_than_12_bits);
                if (!imm.has_value()) {
                    exception = {
                            logic_error("The instruction \""s + instruction_text +
                                        "\" has an invalid immediate value: " + words.at(has_label + 3)),
                            false
                    };
                    return;
                }
                if (greater_than_12_bits) {
                    exception = {
                            logic_error("The instruction \""s + instruction_text +
                                        "\" has an immediate value greater than 12 bits: " + words.at(has_label + 3)),
                            true
                    };
                }
                instruction.operands[2] = imm.value();
                break;
            }
            case S_TYPE: {
                auto reg_index =
                        parse_regindex(words.at(has_label + 1));
                if (!reg_index.has_value()) {
                    exception = {
                            logic_error("The instruction \""s + instruction_text +
                                        "\" has an invalid register name: " + words.at(has_label + 1)),
                            false
                    };
                    return;
                }
                instruction.operands[0] = reg_index.value();
                auto imm_with_offset =
                        parse_immediate_with_offset(words.at(has_label + 2), exception);
                if (!imm_with_offset.has_value()) {
                    return;
                }
                instruction.operands[1] = get<0>(imm_with_offset.value());
                instruction.operands[2] = get<1>(imm_with_offset.value());
                break;
            }
            case B_TYPE: {
                for (int i = 0; i < 2; ++i) {
                    auto reg_index =
                            parse_regindex(words.at(has_label + i + 1));
                    if (!reg_index.has_value()) {
                        exception = {
                                logic_error("The instruction \""s + instruction_text +
                                            "\" has an invalid register name: " + words.at(has_label + i + 1)),
                                false
                        };
                        return;
                    }
                    instruction.operands[i] = reg_index.value();
                }
                instruction.operands[2] = words.at(has_label + 3);
                break;
            }
            case U_TYPE: {
                auto reg_index =
                        parse_regindex(words.at(has_label + 1));
                if (!reg_index.has_value()) {
                    exception = {
                            logic_error("The instruction \""s + instruction_text +
                                        "\" has an invalid register name: " + words.at(has_label + 1)),
                            false
                    };
                    return;
                }
                instruction.operands[0] = reg_index.value();
                bool greater_than_20_bits;
                auto imm =
                        parse_immediate(words.at(has_label + 2), greater_than_20_bits, 0x100000);
                if (!imm.has_value()) {
                    exception = {
                            logic_error("The instruction \""s + instruction_text +
                                        "\" has an invalid immediate value: " + words.at(has_label + 2)),
                            false
                    };
                    return;
                }
                if (greater_than_20_bits) {
                    exception = {
                            logic_error("The instruction \""s + instruction_text +
                                        "\" has an immediate value greater than 20 bits: " + words.at(has_label + 2)),
                            true
                    };
                }
                instruction.operands[1] = imm.value();
                break;
            }
            case J_TYPE: {
                auto reg_index =
                        parse_regindex(words.at(has_label + 1));
                if (!reg_index.has_value()) {
                    exception = {
                            logic_error("The instruction \""s + instruction_text +
                                        "\" has an invalid register name: " + words.at(has_label + 1)),
                            false
                    };
                    return;
                }
                instruction.operands[0] = reg_index.value();
                instruction.operands[1] = words.at(has_label + 2);
                break;
            }
        }
    }
}

namespace exec_fns {
#define EXEC_FN(name) void name (Instruction32 &instruction, function<void(RegIndex_t, RegValue_t)> &set_reg, function<RegValue_t(RegIndex_t)> &get_reg, Memory &memory, RegValue_t &pc)
#define READ_REG_INDX(index) get_reg(get<RegIndex_t>(instruction.operands[index]))
#define READ_IMM(index) get<Instruction32::Immediate_t>(instruction.operands[index])

    EXEC_FN(lui) {
        set_reg(READ_REG_INDX(0), READ_IMM(1) << 12);
    }

    EXEC_FN(auipc) {
        set_reg(READ_REG_INDX(0), (READ_IMM(1) << 12) + pc);
    }

    EXEC_FN(jal) {
        set_reg(READ_REG_INDX(0), pc + 4);
        pc += READ_IMM(1);
    }

    EXEC_FN(jalr) {
        set_reg(READ_REG_INDX(0), pc + 4);
        pc = (READ_IMM(1) + READ_REG_INDX(1)) & 0xFFFFFFFE;
    }

    EXEC_FN(beq) {
        if (READ_REG_INDX(0) == READ_REG_INDX(1)) {
            pc += READ_IMM(2);
        } else {
            pc += 4;
        }
    }

    // declare a static constant array of function pointers
    // to the execution functions for each instruction
    static const array<function<void(Instruction32 &, function<void(RegIndex_t, RegValue_t)>&, function<RegValue_t(RegIndex_t)>&, Memory &, RegValue_t &)>, 37>
            exec_fns = {
    };

}

Instruction32::Instruction32(const string &instruction_text, ParsingException_t &exception,
                             optional<UnresolvedLabel_t> &unresolved_label) {
    // Replace any tab or new line with a space, makes parsing easier
    string inst_text_clone = instruction_text;
    std::for_each(inst_text_clone.begin(), inst_text_clone.end(), [&](auto &item) {
        if (item == '\t' || item == '\n') {
            item = ' ';
        }
    });

    // Split string by space
    vector<string> words;
    int pos = 0;
    while (pos < inst_text_clone.size()) {
        int next_pos = inst_text_clone.find(' ', pos);
        if (next_pos == string::npos) {
            next_pos = inst_text_clone.size();
        }
        words.push_back(inst_text_clone.substr(pos, next_pos - pos));
        pos = next_pos + 1;
    }

    // Remove empty words and commas
    for (int i = 0; i < words.size(); ++i) {
        if (words.at(i).empty() || words.at(i) == ",") {
            words.erase(words.begin() + i);
            --i;
        }
        // remove a comma at the end of a word
        if (words.at(i).at(words.at(i).size() - 1) == ',') {
            words.at(i) = words.at(i).substr(0, words.at(i).size() - 1);
        }
        // remove a comma at the start of a word
        if (words.at(i).at(0) == ',') {
            words.at(i) = words.at(i).substr(1);
        }
    }

    bool has_label = words.at(0).find(":") != string::npos;
    if (has_label) {
        unresolved_label = words.at(0).substr(0, words.at(0).size() - 1);
    } else {
        unresolved_label = nullopt;
    }

    // If the first word in the line contains a colon, then it is a label,
    // so we take the second word as the instruction type. Otherwise we take the first
    if (!has_label) {
        this->type = get_instruction_type(words.at(0));
    } else {
        this->type = get_instruction_type(words.at(1));
    }

    parsing::parse_operands(*this, words, has_label, exception, instruction_text);
}

void Instruction32::execute(function<void(RegIndex_t, RegValue_t)> &set_reg, function<RegValue_t(RegIndex_t)> &get_reg,
                            Memory &memory, RegValue_t &pc) {
    exec_fns::exec_fns[this->type](*this, set_reg, get_reg, memory, pc);
}