cmake_minimum_required(VERSION 3.26)
project(riscv_simulator)

set(CMAKE_CXX_STANDARD 20)

add_executable(riscv_simulator main.cpp RV32I_Instruction.h Instruction32.cpp Instruction32.h RV32I_Instruction.cpp)
