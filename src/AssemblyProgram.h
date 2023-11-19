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
public:
    typedef struct {
        uint32_t address;
        Instruction32 instruction;
    } InstructionWithAddress;

private:
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

    std::array<RegValue_t, 32> get_registers() const { return register_file; };

    const Memory &get_memory_const() const { return memory; };

    Memory &get_memory() { return memory; };

    uint32_t get_pc() const { return pc; }

    const std::vector<InstructionWithAddress> &get_instructions_const() const { return instructions; }

    bool has_reached_end_of_program() const { return reached_end_of_program; }

    bool execute_next_instruction();

    void run();
};


#endif //RISCV_SIMULATOR_ASSEMBLYPROGRAM_H
