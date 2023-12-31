//
// Created by yaman on 13/11/23.
//

#ifndef RISCV_SIMULATOR_INSTRUCTION32_H
#define RISCV_SIMULATOR_INSTRUCTION32_H

#include <cstdint>
#include <string>
#include <variant>
#include <stdexcept>
#include <optional>
#include <functional>

#include "RV32I_Instruction.h"
#include "Register.h"
#include "Memory.h"

class Instruction32 {
public:
    typedef int32_t Immediate_t;
    typedef std::string UnresolvedLabel_t;
    typedef std::variant<RegIndex_t, Immediate_t, UnresolvedLabel_t> Operand_t;
    typedef struct {
        std::optional<std::string> error;
        bool is_warning;
    } ParsingException_t;

    const std::string instruction_text;
    RV32I_Instruction type;
    Operand_t operands[3];

    Instruction32() = default;

    Instruction32(const Instruction32 &other) = default;

    Instruction32(Instruction32 &&other) noexcept = default;

    Instruction32(const std::string &instruction_text, ParsingException_t &error, std::optional<UnresolvedLabel_t>& unresolved_label);

    void execute(
            const std::function<void(RegIndex_t, RegValue_t)>& set_reg,
            const std::function<RegValue_t(RegIndex_t)>& get_reg,
            Memory &memory, RegValue_t &pc);
};

#endif //RISCV_SIMULATOR_INSTRUCTION32_H
