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
            Instruction32::ParsingException_t &error
    ) {
        if (imm_text.find('(') == string::npos || imm_text.find(')') == string::npos) {
            error = {
                    "The instruction \""s + imm_text +
                    "\" has an invalid immediate value: " + imm_text,
                    false
            };
            return nullopt;
        }
        auto imm = parse_immediate(imm_text.substr(0, imm_text.find('(')), error.is_warning);
        if (!imm.has_value()) {
            error = {
                    "The instruction \""s + imm_text +
                    "\" has an invalid immediate value: " + imm_text,
                    false
            };
            return nullopt;
        }
        auto reg_index = parse_regindex(imm_text.substr(imm_text.find('(') + 1, imm_text.find(')') - 1));
        if (!reg_index.has_value()) {
            error = {
                    "The instruction \""s + imm_text +
                    "\" has an invalid register name: " + imm_text,
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
            Instruction32::ParsingException_t &error,
            const string &instruction_text
    ) {
        switch (get_instruction_format(instruction.type)) {
            case R_TYPE: {
                for (int i = 0; i < 3; ++i) {
                    auto reg_index =
                            parse_regindex(words.at(has_label + i + 1));
                    if (!reg_index.has_value()) {
                        error = {
                                "The instruction \""s + instruction_text +
                                "\" has an invalid register name: " + words.at(has_label + i + 1),
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
                        error = {
                                "The instruction \""s + instruction_text +
                                "\" has an invalid register name: " + words.at(has_label + i + 1),
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
                    error = {
                            "The instruction \""s + instruction_text +
                            "\" has an invalid immediate value: " + words.at(has_label + 3),
                            false
                    };
                    return;
                }
                if (greater_than_12_bits) {
                    error = {
                            "The instruction \""s + instruction_text +
                            "\" has an immediate value greater than 12 bits: " + words.at(has_label + 3),
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
                    error = {
                            "The instruction \""s + instruction_text +
                            "\" has an invalid register name: " + words.at(has_label + 1),
                            false
                    };
                    return;
                }
                instruction.operands[0] = reg_index.value();
                auto imm_with_offset =
                        parse_immediate_with_offset(words.at(has_label + 2), error);
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
                        error = {
                                "The instruction \""s + instruction_text +
                                "\" has an invalid register name: " + words.at(has_label + i + 1),
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
                    error = {
                            "The instruction \""s + instruction_text +
                            "\" has an invalid register name: " + words.at(has_label + 1),
                            false
                    };
                    return;
                }
                instruction.operands[0] = reg_index.value();
                bool greater_than_20_bits;
                auto imm =
                        parse_immediate(words.at(has_label + 2), greater_than_20_bits, 0x100000);
                if (!imm.has_value()) {
                    error = {
                            "The instruction \""s + instruction_text +
                            "\" has an invalid immediate value: " + words.at(has_label + 2),
                            false
                    };
                    return;
                }
                if (greater_than_20_bits) {
                    error = {
                            "The instruction \""s + instruction_text +
                            "\" has an immediate value greater than 20 bits: " + words.at(has_label + 2),
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
                    error = {
                            "The instruction \""s + instruction_text +
                            "\" has an invalid register name: " + words.at(has_label + 1),
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
#define READ_REG_INDX(index) get<RegIndex_t>(instruction.operands[index])
#define READ_REG(index) get_reg(READ_REG_INDX(index))
#define READ_IMM(index) get<Instruction32::Immediate_t>(instruction.operands[index])
#define RS1 READ_REG(1)
#define RS2 READ_REG(2)
#define RD_INDX READ_REG_INDX(0)
#define RS1_INDX READ_REG_INDX(1)
#define RS2_INDX READ_REG_INDX(2)
#define IMM1 READ_IMM(1)
#define IMM2 READ_IMM(2)

    //TODO: Might have to deal with Endianness of x86

    EXEC_FN(LUI) {
        set_reg(RD_INDX, IMM1 << 12);
    }

    EXEC_FN(AUIPC) {
        set_reg(RD_INDX, (IMM1 << 12) + pc);
    }

    EXEC_FN(JAL) {
        set_reg(RD_INDX, pc + 4);
        pc += (int32_t) IMM1;
    }

    EXEC_FN(JALR) {
        set_reg(RD_INDX, pc + 4);
        pc = ((int32_t) IMM1 + READ_REG(2)) & 0xFFFFFFFE;
    }

    EXEC_FN(BEQ) {
        if (READ_REG(0) == READ_REG(1)) {
            pc += (int32_t) IMM2;
        }
    }

    EXEC_FN(BNE) {
        if (READ_REG(0) != READ_REG(1)) {
            pc += (int32_t) IMM2;
        }
    }

    EXEC_FN(BLT) {
        if ((int32_t) READ_REG(0) < (int32_t) READ_REG(1)) {
            pc += (int32_t) IMM2;
        }
    }

    EXEC_FN(BGE) {
        if ((int32_t) READ_REG(0) >= (int32_t) READ_REG(1)) {
            pc += (int32_t) IMM2;
        }
    }

    EXEC_FN(BLTU) {
        if (READ_REG(0) < READ_REG(1)) {
            pc += (int32_t) IMM2;
        }
    }

    EXEC_FN(BGEU) {
        if (READ_REG(0) >= READ_REG(1)) {
            pc += (int32_t) IMM2;
        }
    }

    EXEC_FN(LB) {
        set_reg(RD_INDX, (int32_t) memory.get_byte(READ_REG(1) + (int32_t) IMM1));
    }

    EXEC_FN(LH) {
        set_reg(RD_INDX, (int32_t) memory.get_half_word(READ_REG(1) + (int32_t) IMM1));
    }

    EXEC_FN(LW) {
        set_reg(RD_INDX, memory.get_word(READ_REG(1) + (int32_t) IMM1));
    }

    EXEC_FN(LBU) {
        set_reg(RD_INDX, memory.get_byte(READ_REG(1) + (int32_t) IMM1));
    }

    EXEC_FN(LHU) {
        set_reg(RD_INDX, memory.get_half_word(READ_REG(1) + (int32_t) IMM1));
    }

    EXEC_FN(SB) {
        memory.set_byte(READ_REG(2) + (int32_t) IMM1, (uint8_t) READ_REG(0));
    }

    EXEC_FN(SH) {
        memory.set_half_word(READ_REG(2) + (int32_t) IMM1, (uint16_t) READ_REG(0));
    }

    EXEC_FN(SW) {
        memory.set_word(READ_REG(2) + (int32_t) IMM1, READ_REG(0));
    }

    EXEC_FN(ADDI) {
        set_reg(RD_INDX, (int32_t) READ_REG(1) + (int32_t) IMM2);
    }

    EXEC_FN(SLTI) {
        set_reg(RD_INDX, (int32_t) READ_REG(1) < (int32_t) IMM2);
    }

    EXEC_FN(SLTIU) {
        set_reg(RD_INDX, READ_REG(1) < IMM2);
    }

    EXEC_FN(XORI) {
        set_reg(RD_INDX, READ_REG(1) ^ IMM2);
    }

    EXEC_FN(ORI) {
        set_reg(RD_INDX, READ_REG(1) | IMM2);
    }

    EXEC_FN(ANDI) {
        set_reg(RD_INDX, READ_REG(1) & IMM2);
    }

    EXEC_FN(SLLI) {
        set_reg(RD_INDX, READ_REG(1) << IMM2);
    }

    EXEC_FN(SRLI) {
        set_reg(RD_INDX, READ_REG(1) >> IMM2);
    }

    EXEC_FN(SRAI) {
        set_reg(RD_INDX, (int32_t) READ_REG(1) >> IMM2);
    }

    EXEC_FN(ADD) {
        set_reg(RD_INDX, (int32_t) READ_REG(1) + (int32_t) READ_REG(2));
    }

    EXEC_FN(SUB) {
        set_reg(RD_INDX, (int32_t) READ_REG(1) - (int32_t) READ_REG(2));
    }

    EXEC_FN(SLL) {
        set_reg(RD_INDX, READ_REG(1) << (READ_REG(2) & 0x1F));
    }

    EXEC_FN(SLT) {
        set_reg(RD_INDX, (int32_t) READ_REG(1) < (int32_t) READ_REG(2));
    }

    EXEC_FN(SLTU) {
        set_reg(RD_INDX, READ_REG(1) < READ_REG(2));
    }

    EXEC_FN(XOR) {
        set_reg(RD_INDX, READ_REG(1) ^ READ_REG(2));
    }

    EXEC_FN(SRL) {
        set_reg(RD_INDX, READ_REG(1) >> (READ_REG(2) & 0x1F));
    }

    EXEC_FN(SRA) {
        set_reg(RD_INDX, (int32_t) READ_REG(1) >> (READ_REG(2) & 0x1F));
    }

    EXEC_FN(OR) {
        set_reg(RD_INDX, READ_REG(1) | READ_REG(2));
    }

    EXEC_FN(AND) {
        set_reg(RD_INDX, READ_REG(1) & READ_REG(2));
    }

#undef EXEC_FN
#undef READ_REG_INDX
#undef READ_REG
#undef READ_IMM
#undef RS1
#undef RS2
#undef RD_INDX
#undef RS1_INDX
#undef RS2_INDX
#undef IMM1
#undef IMM2
}

Instruction32::Instruction32(const string &instruction_text, ParsingException_t &error,
                             optional<UnresolvedLabel_t> &unresolved_label)
        : instruction_text(instruction_text) {
    // Replace any tab or new line with a space, makes parsing easier
    error = {nullopt, false};
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
        bool word_empty = words.at(i).empty();
        if (word_empty || words.at(i) == ",") {
            words.erase(words.begin() + i);
        }
        // remove a comma at the end of a word
        if (words.at(i).at(words.at(i).size() - 1) == ',') {
            words.at(i) = words.at(i).substr(0, words.at(i).size() - 1);
        }
        // remove a comma at the start of a word
        if (words.at(i).at(0) == ',') {
            words.at(i) = words.at(i).substr(1);
        }
        if (word_empty) {
            i--;
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
    if (words.size() < 3 + has_label) {
        if (has_label && words.size() == 1)
            error = {
                    "The instruction \""s + instruction_text + "\" only has a label",
                    true
            };
        else
            error = {
                    "The instruction \""s + instruction_text + "\" has too few operands",
                    true
            };
        return;
    }

    optional<std::string> ex;
    if (auto x = get_instruction_type(words.at(0 + has_label), ex)) {
        this->type = x.value();
    } else {
        error = {ex.value(), false};
        return;
    }

    parsing::parse_operands(*this, words, has_label, error, instruction_text);
}

void Instruction32::execute(function<void(RegIndex_t, RegValue_t)> &set_reg, function<RegValue_t(RegIndex_t)> &get_reg,
                            Memory &memory, RegValue_t &pc) {

    // If we still have an unresolved label, we can't work with it
    if (holds_alternative<UnresolvedLabel_t>(this->operands[2])) {
        throw logic_error("The instruction \""s + this->instruction_text +
                          "\" has an unresolved label: " + get<UnresolvedLabel_t>(this->operands[2]));
    }

#define EXEC_CASE(name) case name: exec_fns::name(*this, set_reg, get_reg, memory, pc); break;
    switch (this->type) {
        EXEC_CASE(LUI)
        EXEC_CASE(AUIPC)
        EXEC_CASE(JAL)
        EXEC_CASE(JALR)
        EXEC_CASE(BEQ)
        EXEC_CASE(BNE)
        EXEC_CASE(BLT)
        EXEC_CASE(BGE)
        EXEC_CASE(BLTU)
        EXEC_CASE(BGEU)
        EXEC_CASE(LB)
        EXEC_CASE(LH)
        EXEC_CASE(LW)
        EXEC_CASE(LBU)
        EXEC_CASE(LHU)
        EXEC_CASE(SB)
        EXEC_CASE(SH)
        EXEC_CASE(SW)
        EXEC_CASE(ADDI)
        EXEC_CASE(SLTI)
        EXEC_CASE(SLTIU)
        EXEC_CASE(XORI)
        EXEC_CASE(ORI)
        EXEC_CASE(ANDI)
        EXEC_CASE(SLLI)
        EXEC_CASE(SRLI)
        EXEC_CASE(SRAI)
        EXEC_CASE(ADD)
        EXEC_CASE(SUB)
        EXEC_CASE(SLL)
        EXEC_CASE(SLT)
        EXEC_CASE(SLTU)
        EXEC_CASE(XOR)
        EXEC_CASE(SRL)
        EXEC_CASE(SRA)
        EXEC_CASE(OR)
        EXEC_CASE(AND)
    }
#undef EXEC_CASE
}
