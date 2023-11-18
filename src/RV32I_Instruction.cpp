//
// Created by yaman on 13/11/23.
//
#include <string>
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include "RV32I_Instruction.h"

using namespace std;


RV32I_Instruction get_instruction_type(std::string instruction_text) {
    transform(
            instruction_text.begin(),
            instruction_text.end(),
            instruction_text.begin(),
            ::toupper
    );
    return rv32i_string_to_enum_conversion_map.at(instruction_text);
}

Instruction32Format get_instruction_format(const RV32I_Instruction &instruction) {
    switch (instruction) {
        case LUI:
        case AUIPC:
            return U_TYPE;
        case JAL:
            return J_TYPE;
        case BEQ:
        case BNE:
        case BLT:
        case BGE:
        case BLTU:
        case BGEU:
            return B_TYPE;
        case LB:
        case LH:
        case LW:
        case LBU:
        case LHU:
        case ADDI:
        case SLTI:
        case SLTIU:
        case XORI:
        case ORI:
        case ANDI:
        case SLLI:
        case SRLI:
        case SRAI:
        case JALR:
            return I_TYPE;
        case SB:
        case SH:
        case SW:
            return S_TYPE;
        case ADD:
        case SUB:
        case SLL:
        case SLT:
        case SLTU:
        case XOR:
        case SRL:
        case SRA:
        case OR:
        case AND:
            return R_TYPE;
    }
}
