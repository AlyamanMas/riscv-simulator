//
// Created by yaman on 17/11/23.
//

#ifndef RISCV_SIMULATOR_MEMORY_H
#define RISCV_SIMULATOR_MEMORY_H

#include <map>
#include <cstdint>
#include <string>
#include <variant>

class Memory {
public:
    typedef uint32_t Address_t;

    Memory() = default;

    Memory(std::string memory_text);

    uint32_t get_word(uint32_t address) const;

    uint16_t get_half_word(uint32_t address) const;

    uint8_t get_byte(uint32_t address) const;

    void set_word(uint32_t address, uint32_t value);

    void set_half_word(uint32_t address, uint16_t value);

    void set_byte(uint32_t address, uint8_t value);

//    std::string serialize();
    const std::map<Address_t, uint8_t> &get_const_reference() { return memory_map; };

private:
    typedef std::tuple<Address_t, std::variant<uint32_t, uint16_t, uint8_t, std::string>> MemoryOperation_t;

    std::map<Address_t, uint8_t> memory_map;

    MemoryOperation_t deserialize_memory_operation(std::string memory_operation_text);

    void execute_memory_operation(MemoryOperation_t memory_operation);
};


#endif //RISCV_SIMULATOR_MEMORY_H
