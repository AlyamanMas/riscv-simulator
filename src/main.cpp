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

vector<string> read_file_into_vector(filesystem::path path) {
    ifstream file(path);
    vector<string> lines;
    for (string line; getline(file, line);) {
        lines.push_back(line);
    }
    return lines;
}

int main() {
    auto project_dir = filesystem::path(__FILE__).parent_path().parent_path() / "asm-tests";

    // prompt user for file name
    string prog_filename;
    cout << "Enter the name of the program file: ";
    cin >> prog_filename;
    auto test_file = project_dir / prog_filename;

    // promput user for memory file name
    string mem_filename;
    cout << "Enter the name of the memory file: ";
    cin >> mem_filename;
    auto mem_file = project_dir / mem_filename;

    auto my_program = AssemblyProgram(read_file(test_file));
    my_program.get_memory().execute_memory_string(read_file_into_vector(mem_file));
    my_program.run();
    return 0;
}
