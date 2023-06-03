cmake_minimum_required(VERSION 3.21)
project(OpenMPClasses C)

set(CMAKE_C_STANDARD 99)

add_executable(stampedValue.c stampedValue.h
        mrmwRegisterOpenMP.c mrmwRegisterOpenMP.h)