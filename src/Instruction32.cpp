//
// Created by yaman on 13/11/23.
//

#include "Instruction32.h"

#include <algorithm>
#include <vector>
#include <optional>

#include "RV32I_Instruction.h"

using namespace std;

static std::unordered_map<std::string, Instruction32::RegIndex_t> const register_convention_map = {
        {"zero", 0},
        {"ra", 1},
        {"sp", 2},
        {"gp", 3},
        {"tp", 4},
        {"t0", 5},
        {"t1", 6},
        {"t2", 7},
        {"s0", 8},
        {"fp", 8},
        {"s1", 9},
        {"a0", 10},
        {"a1", 11},
        {"a2", 12},
        {"a3", 13},
        {"a4", 14},
        {"a5", 15},
        {"a6", 16},
        {"a7", 17},
        {"s2", 18},
        {"s3", 19},
        {"s4", 20},
        {"s5", 21},
        {"s6", 22},
        {"s7", 23},
        {"s8", 24},
        {"s9", 25},
        {"s10", 26},
        {"s11", 27},
        {"t3", 28},
        {"t4", 29},
        {"t5", 30},
        {"t6", 31}
};

// note reg_text must not have a leading or trailing comma or space,
// it should exactly be in the format 'xi' where i is a number between 0 and 31
// or a register convention name
optional<Instruction32::RegIndex_t> parse_regindex(const string &reg_text) {
    if (reg_text.at(0) == 'x') {
        if (reg_text.size() == 2 && isdigit(reg_text.at(1))) {
            return optional<Instruction32::RegIndex_t>(reg_text.at(1) - '0');
        } else if (reg_text.size() == 3 && isdigit(reg_text.at(1)) && isdigit(reg_text.at(2))) {
            if (stoi(reg_text.substr(1)) > 31) {
                return nullopt;
            }
            return optional<Instruction32::RegIndex_t>((reg_text.at(1) - '0') * 10 + (reg_text.at(2) - '0'));
        }
    } else if (register_convention_map.contains(reg_text)) {
        return optional<Instruction32::RegIndex_t>(register_convention_map.at(reg_text));
    }
    return nullopt;
}

Instruction32::Instruction32(string instruction_text) {
    // Replace any tab or new line with a space, makes parsing easier
    std::for_each(instruction_text.begin(), instruction_text.end(), [&](auto &item) {
        if (item == '\t' || item == '\n') {
            item = ' ';
        }
    });

    // Split string by space
    vector<string> words;
    int pos = 0;
    while (pos < instruction_text.size()) {
        int next_pos = instruction_text.find(' ', pos);
        if (next_pos == string::npos) {
            next_pos = instruction_text.size();
        }
        words.push_back(instruction_text.substr(pos, next_pos - pos));
        pos = next_pos + 1;
    }

    // Remove empty words and commas
    for (int i = 0; i < words.size(); ++i) {
        if (words.at(i).empty() || words.at(i) == ",") {
            words.erase(words.begin() + i);
            --i;
        }
        // remove a comma at the end of a word
        if (words.at(i).at(words.at(i).size() - 1) == ',') {
            words.at(i) = words.at(i).substr(0, words.at(i).size() - 1);
        }
        // remove a comma at the start of a word
        if (words.at(i).at(0) == ',') {
            words.at(i) = words.at(i).substr(1);
        }
    }

    // If the first word in the line contains a colon, then it is a label,
    // so we take the second word as the instruction type. Otherwise we take the first
    if (words.at(0).find(':') == string::npos) {
        this->type = get_instruction_type(words.at(0));
    } else {
        this->type = get_instruction_type(words.at(1));
    }

}
