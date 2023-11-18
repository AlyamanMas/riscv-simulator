//
// Created by yaman on 17/11/23.
//

#ifndef RISCV_SIMULATOR_ASSEMBLYPROGRAM_H
#define RISCV_SIMULATOR_ASSEMBLYPROGRAM_H

#include <variant>
#include <array>

#include "Register.h"
#include "Memory.h"
#include "Instruction32.h"

typedef std::string Label_t;

class AssemblyProgram {
    RegisterFile_t register_file;
    RegValue_t pc;
    Memory memory;
    std::vector<std::variant<Label_t, Instruction32>> instructions;
    int top_instruction_index;
public:
    AssemblyProgram(std::string program_text);

    void set_register(RegIndex_t index, RegValue_t value);

    RegValue_t get_register(RegIndex_t index) const;

    void run();
};


#endif //RISCV_SIMULATOR_ASSEMBLYPROGRAM_H
