//
// Created by yaman on 17/11/23.
//

#include "Memory.h"

#include <vector>
#include <string>
#include <sstream>

using namespace std;

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
    if (memory_map.find(address) == memory_map.end()) {
        return 0;
    }
    return memory_map.at(address);
}

uint16_t Memory::get_half_word(uint32_t address) const {
    return (get_byte(address + 1) << 8) |
           get_byte(address);
}

uint32_t Memory::get_word(uint32_t address) const {
    return (get_byte(address + 3) << 24) |
           (get_byte(address + 2) << 16) |
           (get_byte(address + 1) << 8) |
           get_byte(address);
}

// each memory operation in the vector is a complete operation
Memory::MemoryOperation_t Memory::deserialize_memory_operation(string memory_op_str) {
    // the format of each element in the vector is similar to the below:
    // first element is the address, it could be empty
    // second element is the type, it could be string, byte, half, word
    // then the elements
    // "0x1000, string, Hello world, Wow, this is a string"
    // "0x1000, byte, 0x12, 0x34, 0x56, 0x78"
    // "0x1000, half, 0x1234, 0x5678"
    // "0x1000, word, 0x12345678"

    MemoryOperation_t memory_operation;
    istringstream ss(memory_op_str);
    string address, type;
    vector<string> data;

    getline(ss, address, ',');
    getline(ss, type, ',');
    string temp;
    while (getline(ss, temp, ',')) {
        data.push_back(temp);
    }

    // trim address and type
    address.erase(0, address.find_first_not_of(" \t\n\r\f\v"));
    address.erase(address.find_last_not_of(" \t\n\r\f\v") + 1);
    type.erase(0, type.find_first_not_of(" \t\n\r\f\v"));
    type.erase(type.find_last_not_of(" \t\n\r\f\v") + 1);
    // trim the data
    for (auto &str : data) {
        str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
        str.erase(str.find_last_not_of(" \t\n\r\f\v") + 1);
    }

    if (address.empty()) {
        memory_operation.address = nullopt;
    } else {
        memory_operation.address = stoul(address, nullptr, 16);
    }

    if (type == "string") {
        memory_operation.data = data;
    } else if (type == "byte") {
        vector<uint8_t> byte_data;
        for (auto &str : data) {
            byte_data.push_back(stoul(str, nullptr, 16));
        }
        memory_operation.data = byte_data;
    } else if (type == "half") {
        vector<uint16_t> half_data;
        for (auto &str : data) {
            half_data.push_back(stoul(str, nullptr, 16));
        }
        memory_operation.data = half_data;
    } else if (type == "word") {
        vector<uint32_t> word_data;
        for (auto &str : data) {
            word_data.push_back(stoul(str, nullptr, 16));
        }
        memory_operation.data = word_data;
    } else {
        throw runtime_error("Invalid memory operation type: " + type);
    }

    return memory_operation;
}

void save_data(const vector<string> &str_vec, uint32_t &address, Memory &memory) {
    for (auto &str : str_vec) {
        for (auto &c : str) {
            memory.set_byte(address++, c);
        }
        memory.set_byte(address++, '\0');
    }
}

void save_data(const vector<uint8_t> &byte_vec, uint32_t &address, Memory &memory) {
    for (auto &byte : byte_vec) {
        memory.set_byte(address++, byte);
    }
}

void save_data(const vector<uint16_t> &half_vec, uint32_t &address, Memory &memory) {
    for (auto &half : half_vec) {
        memory.set_half_word(address, half);
        address += 2;
    }
}

void save_data(const vector<uint32_t> &word_vec, uint32_t &address, Memory &memory) {
    for (auto &word : word_vec) {
        memory.set_word(address, word);
        address += 4;
    }
}

void Memory::execute_memory_operation(const Memory::MemoryOperation_t &memory_operation) {
    uint32_t address = memory_operation.address.value_or(0);
    if (memory_operation.data.index() == 0) {
        save_data(get<vector<uint32_t>>(memory_operation.data), address, *this);
    } else if (memory_operation.data.index() == 1) {
        save_data(get<vector<uint16_t>>(memory_operation.data), address, *this);
    } else if (memory_operation.data.index() == 2) {
        save_data(get<vector<uint8_t>>(memory_operation.data), address, *this);
    } else if (memory_operation.data.index() == 3) {
        save_data(get<vector<string>>(memory_operation.data), address, *this);
    } else {
        throw runtime_error("Invalid memory operation data type");
    }
}

void Memory::execute_memory_string(const vector<std::string> &operations_str) {
    for (auto &memory_op_str : operations_str) {
        execute_memory_operation(deserialize_memory_operation(memory_op_str));
    }
}

Memory::Memory(std::string memory_text) {
    istringstream ss(memory_text);
    string line;
    vector<string> operations_str;
    while (getline(ss, line)) {
        operations_str.push_back(line);
    }
    execute_memory_string(operations_str);
}
