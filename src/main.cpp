#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "AssemblyProgram.h"

using namespace std;

string read_file(filesystem::path path) {
    ifstream file(path);
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    auto project_dir = filesystem::path(__FILE__).parent_path().parent_path() / "asm-tests";
    auto test_file = project_dir / "riscv1.asm";

    auto my_program = AssemblyProgram(read_file(test_file));
    return 0;
}
