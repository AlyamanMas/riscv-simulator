//
// Created by yaman on 17/11/23.
//

#include "Memory.h"

void Memory::set_byte(uint32_t address, uint8_t value) {
    memory_map[address] = value;
}

void Memory::set_half_word(uint32_t address, uint16_t value) {
    memory_map[address] = value & 0xFF;
    memory_map[address + 1] = (value >> 8) & 0xFF;
}

void Memory::set_word(uint32_t address, uint32_t value) {
    memory_map[address] = value & 0xFF;
    memory_map[address + 1] = (value >> 8) & 0xFF;
    memory_map[address + 2] = (value >> 16) & 0xFF;
    memory_map[address + 3] = (value >> 24) & 0xFF;
}

uint8_t Memory::get_byte(uint32_t address) const {
    return memory_map.at(address);
}

uint16_t Memory::get_half_word(uint32_t address) const {
    return (memory_map.at(address + 1) << 8) |
           memory_map.at(address);
}

uint32_t Memory::get_word(uint32_t address) const {
    return (memory_map.at(address + 3) << 24) |
           (memory_map.at(address + 2) << 16) |
           (memory_map.at(address + 1) << 8) |
           memory_map.at(address);
}
