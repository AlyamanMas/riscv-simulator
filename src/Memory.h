//
// Created by yaman on 17/11/23.
//

#ifndef RISCV_SIMULATOR_MEMORY_H
#define RISCV_SIMULATOR_MEMORY_H

#include <map>
#include <cstdint>
#include <string>
#include <variant>
#include <optional>
#include <vector>

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
    // Memory operation is a pair of address and data
    // if the address isn't specified, we assume it's the next available address
    typedef struct {
        std::optional<Address_t> address;
        std::variant<
                std::vector<uint32_t>,
                std::vector<uint16_t>,
                std::vector<uint8_t>,
                std::vector<std::string>
        > data;
    } MemoryOperation_t;

    std::map<Address_t, uint8_t> memory_map;

    MemoryOperation_t deserialize_memory_operation(std::string memory_op_str);

    void execute_memory_operation(const MemoryOperation_t &memory_operation);

    void execute_memory_string(const std::vector<std::string>& operations_str);
};


#endif //RISCV_SIMULATOR_MEMORY_H
