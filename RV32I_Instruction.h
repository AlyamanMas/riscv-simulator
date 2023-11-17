//
// Created by yaman on 13/11/23.
//

#ifndef RISCV_SIMULATOR_RV32I_INSTRUCTION_H
#define RISCV_SIMULATOR_RV32I_INSTRUCTION_H

#include <unordered_map>

typedef enum {
    LUI,
    AUIPC,
    JAL,
    JALR,
    BEQ,
    BNE,
    BLT,
    BGE,
    BLTU,
    BGEU,
    LB,
    LH,
    LW,
    LBU,
    LHU,
    SB,
    SH,
    SW,
    ADDI,
    SLTI,
    SLTIU,
    XORI,
    ORI,
    ANDI,
    SLLI,
    SRLI,
    SRAI,
    ADD,
    SUB,
    SLL,
    SLT,
    SLTU,
    XOR,
    SRL,
    SRA,
    OR,
    AND
} RV32I_Instruction;

typedef enum {
    R_TYPE,
    I_TYPE,
    S_TYPE,
    B_TYPE,
    U_TYPE,
    J_TYPE
} InstructionFormat;

static std::unordered_map<std::string, RV32I_Instruction>
const rv32i_string_to_enum_conversion_map = {
        {"LUI",   LUI,},
        {"AUIPC", AUIPC,},
        {"JAL",   JAL,},
        {"JALR",  JALR,},
        {"BEQ",   BEQ,},
        {"BNE",   BNE,},
        {"BLT",   BLT,},
        {"BGE",   BGE,},
        {"BLTU",  BLTU,},
        {"BGEU",  BGEU,},
        {"LB",    LB,},
        {"LH",    LH,},
        {"LW",    LW,},
        {"LBU",   LBU,},
        {"LHU",   LHU,},
        {"SB",    SB,},
        {"SH",    SH,},
        {"SW",    SW,},
        {"ADDI",  ADDI,},
        {"SLTI",  SLTI,},
        {"SLTIU", SLTIU,},
        {"XORI",  XORI,},
        {"ORI",   ORI,},
        {"ANDI",  ANDI,},
        {"SLLI",  SLLI,},
        {"SRLI",  SRLI,},
        {"SRAI",  SRAI,},
        {"ADD",   ADD,},
        {"SUB",   SUB,},
        {"SLL",   SLL,},
        {"SLT",   SLT,},
        {"SLTU",  SLTU,},
        {"XOR",   XOR,},
        {"SRL",   SRL,},
        {"SRA",   SRA,},
        {"OR",    OR,},
        {"AND",   AND}
};

RV32I_Instruction get_instruction_type(std::string instruction_text);

InstructionFormat get_instruction_format(const RV32I_Instruction &instruction);

#endif //RISCV_SIMULATOR_RV32I_INSTRUCTION_H
