/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"


/* Implementation of the Statement class */

int stringToInt(std::string str);

Statement::Statement() = default;

Statement::~Statement() = default;

void REM::execute(Program &program) {}

void LIST::execute(Program &program) {
    for (auto iter = program.lineRef.begin(); iter != program.lineRef.end(); ++iter)
        try { std::cout << (program.lineSource[*iter]) << '\n'; }
        catch (...) { throw; }

}

void RUN::execute(Program &program) {
    auto iter = program.lineRef.begin();
    while (iter != program.lineRef.end()) {
        try { (program.linePoint[*iter])->execute(program); }
        catch (...) { throw; }
        if (program.lineGoto != 0) {
            iter = program.lineRef.find(program.lineGoto);//若行转移，改变iter
            program.lineGoto = 0;
            continue;
        }
        if (program.stop) {
            program.stop = false;//遇到END,停止
            break;
        }
        ++iter;//否则顺序进行
    }
}

void CLEAR::execute(Program &program) {
    program.statePoint->Clear();
    program.clear();
}

void PRINT::execute(Program &program) {
    try {
        std::cout << exp->eval(*(program.statePoint)) << '\n';
    } catch (...) { throw; }
}

void END::execute(Program &program) {
    if (!program.runAlready) {//需要将程序运行一遍
        Statement *stmt = new RUN;
        stmt->execute(program);//立即执行，不需存储
        delete stmt;//释放空间
    }
    program.stop = true;
}


void LET::execute(Program &program) {
    try { (program.statePoint)->setValue(var, exp->eval(*(program.statePoint))); }
    catch (...) { throw; }
}


void INPUT::execute(Program &program) {
    int value;
    while (!program.inputStream.empty()) {
        //每一行输入，若有错误直接处理,删去用过的输入
        std::cout << " ? ";
        try {
            value = stringToInteger(program.inputStream.front());//尝试
        } catch (...) {
            program.inputStream.pop();
            std::cout << "INVALID NUMBER\n";//错误输入
            continue;
        }
        program.inputStream.pop();
        (program.statePoint)->setValue(var, value);
        break;
    }
}

void GOTO::execute(Program &program) {
    if (program.lineRef.count(lineTo) == 0) {
        error("LINE NUMBER ERROR");
    } else {
        program.lineGoto = lineTo;
    }
}

void IF::execute(Program &program) {
    int valueFirst, valueSecond;
    try {
        valueFirst = expFirst->eval(*(program.statePoint));
        valueSecond = expSecond->eval(*(program.statePoint));
    } catch (...) { throw; }
    if ((cmp == "<" && valueFirst < valueSecond) ||
        (cmp == "=" && valueFirst == valueSecond) ||
        (cmp == ">" && valueFirst > valueSecond)) {
        if (program.lineRef.count(lineTo) == 0) {
            error("LINE NUMBER ERROR");
        } else {
            program.lineGoto = lineTo;
        }
    }
}