#include <iostream>

#include "Instruction32.h"

using namespace std;

int main() {
    Instruction32 instruction("\t .L1: ADDI x1 x2 0x123 #comment two words \t");
    cout << "hello world" << endl;
    return 0;
}
