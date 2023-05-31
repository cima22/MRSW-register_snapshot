cmake_minimum_required(VERSION 3.21)
project(project C)

set(CMAKE_C_STANDARD 99)

add_executable(project src/library.c src/mrswRegister.c src/stampedValue.c src/stampedValue.h src/srswRegister.c)
