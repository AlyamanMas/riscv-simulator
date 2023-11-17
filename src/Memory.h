//
// Created by yaman on 17/11/23.
//

#ifndef RISCV_SIMULATOR_MEMORY_H
#define RISCV_SIMULATOR_MEMORY_H

#include <map>
#include <cstdint>

class Memory {
    std::map<uint32_t, uint8_t> memory_map;
public:
    Memory();

    uint32_t get_word(uint32_t address);
    uint16_t get_half_word(uint32_t address);
    uint8_t get_byte(uint32_t address);

    void set_word(uint32_t address, uint32_t value);
    void set_half_word(uint32_t address, uint16_t value);
    void set_byte(uint32_t address, uint8_t value);
};


#endif //RISCV_SIMULATOR_MEMORY_H
