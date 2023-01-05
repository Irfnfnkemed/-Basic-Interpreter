/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include <queue>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"


/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);

/* Main program */
std::queue<std::string> lineRead;//存储input语句在判断输入结束后多读的语句，被program声明为extern变量
int main() {
    EvalState state;
    Program program;
    //cout << "Stub implementation of BASIC" << endl;
    while (true) {
        try {
            std::string input;
            if (lineRead.empty()) { getline(std::cin, input); }
            else {//处理input过程提前读入的语句
                input = lineRead.front();
                lineRead.pop();
            }
            if (input.empty())
                return 0;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(std::string line, Program &program, EvalState &state) {
    int lineNumber;
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);
    std::string token = scanner.nextToken();
    if (token == "") {}//空行，忽略
    else {
        if (scanner.getTokenType(token) == NUMBER) {//有行数，记录行数
            lineNumber = stringToInteger(token);
        } else { lineNumber = -1; }//标记没有行数的语句
        program.statePoint = &state;
        program.addSourceLine(lineNumber, line);
    }
}



