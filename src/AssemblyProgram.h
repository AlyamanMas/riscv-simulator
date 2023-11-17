//
// Created by yaman on 17/11/23.
//

#ifndef RISCV_SIMULATOR_ASSEMBLYPROGRAM_H
#define RISCV_SIMULATOR_ASSEMBLYPROGRAM_H

#include <variant>

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

    void run();
};


#endif //RISCV_SIMULATOR_ASSEMBLYPROGRAM_H
