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
    typedef struct {
        uint32_t address;
        Instruction32 instruction;
    } InstructionWithAddress;

    RegisterFile_t register_file;
    RegValue_t pc;
    Memory memory;
    std::vector<InstructionWithAddress> instructions;
    std::map<Label_t, uint32_t> label_map;
    int top_instruction_address;
    bool reached_end_of_program;

    void resolve_labels();
public:
    AssemblyProgram(std::string program_text, uint32_t starting_address = 0);

    void set_register(RegIndex_t index, RegValue_t value);

    RegValue_t get_register(RegIndex_t index) const;

    void run();
};


#endif //RISCV_SIMULATOR_ASSEMBLYPROGRAM_H
