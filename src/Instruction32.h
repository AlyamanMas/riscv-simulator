//
// Created by yaman on 13/11/23.
//

#ifndef RISCV_SIMULATOR_INSTRUCTION32_H
#define RISCV_SIMULATOR_INSTRUCTION32_H

#include <cstdint>
#include <string>
#include <variant>

#include "RV32I_Instruction.h"
#include "Register.h"

class Instruction32 {
public:
    typedef uint16_t Immediate_t;
    typedef std::string UnresolvedLabel_t;
    typedef enum {
        RegIndex,
        Immediate,
        UnresolvedLabel,
    } OperandType;

    RV32I_Instruction type;
    std::variant<RegIndex_t, Immediate_t, UnresolvedLabel_t> operands[3];

    Instruction32(std::string instruction_text);
};

#endif //RISCV_SIMULATOR_INSTRUCTION32_H
