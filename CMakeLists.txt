cmake_minimum_required(VERSION 3.16)
project(code)

set(CMAKE_CXX_STANDARD 20)

add_executable(code
        Basic.cpp
        evalstate.cpp
        exp.cpp
        parser.cpp
        program.cpp
        statement.cpp
        Utils/error.cpp Utils/error.hpp Utils/tokenScanner.cpp Utils/tokenScanner.hpp
        Utils/strlib.cpp
        )
