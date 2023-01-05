/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.hpp"


Program::Program() = default;

Program::~Program() {
    clear();
}

void Program::clear() {
    extern std::queue<std::string> lineRead;
    for (auto iter = lineRef.begin(); iter != lineRef.end(); ++iter) {
        delete linePoint[*iter];
    }
    lineRef.clear();
    lineSource.clear();
    linePoint.clear();
    runAlready = false;
    while (!lineRead.empty()) { lineRead.pop(); }
    lineGoto = 0;
    while (!inputStream.empty()) { inputStream.pop(); }
    stop = false;

}

void Program::addSourceLine(int lineNumber, const std::string &line) {
    removeSourceLine(lineNumber);//移除之前的信息（若存在）
    if (lineNumber == -1) {//需立即执行的语句
        lineSource[-1] = line;//插入行内容
        Statement *stmt;
        stmt = getParsedStatement(lineNumber);
        try {
            stmt->execute(*this);//立即执行，不需存储
        } catch (...) {
            delete stmt;//释放getParsedStatement函数申请空间
            throw;//将捕获到的继续抛出
        }
        delete stmt;
        lineSource.erase(-1);
    } else if (line != integerToString(lineNumber)) {//除行数外不为空，记录
        lineRef.insert(lineNumber);//插入行编号
        lineSource[lineNumber] = line;//插入行内容
        try {
            linePoint[lineNumber] = getParsedStatement(lineNumber);//插入对应的解析的指针
        } catch (...) { throw; }
    } else {}//只有行数，表删除，不重新添加内容
}

void Program::removeSourceLine(int lineNumber) {
    if (lineSource.count(lineNumber) == 1) {
        lineRef.erase(lineNumber);
        lineSource.erase(lineNumber);
        delete linePoint[lineNumber];
        linePoint.erase(lineNumber);
    }
}

std::string Program::getSourceLine(int lineNumber) {
    if (lineSource.count(lineNumber) == 1) {
        return lineSource[lineNumber];
    } else { return ""; }
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    if (lineSource.count(lineNumber) == 1) {
        linePoint[lineNumber] = stmt;
    } else { error("LINE NUMBER ERROR"); }
}


Statement *Program::getParsedStatement(int lineNumber) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(lineSource[lineNumber]);
    std::string token = scanner.nextToken();
    if (scanner.getTokenType(token) == NUMBER) {//去除前面可能的行号
        token = scanner.nextToken();
    }
    if (token == "REM") {
        Statement *stmt;
        return stmt = new REM;
    }
    if (token == "LIST") {
        Statement *stmt;
        return stmt = new LIST;
    }
    if (token == "RUN") {
        runAlready = true;
        Statement *stmt;
        return stmt = new RUN;
        stmt->execute(*this);//立即执行，不需存储
        delete stmt;//释放空间
    }
    if (token == "CLEAR") {
        Statement *stmt;
        return stmt = new CLEAR;
    }
    if (token == "QUIT") {//退出具有特殊性，需特判
        if (!runAlready) {//需要将程序运行一遍
            Statement *stmt = new RUN;
            stmt->execute(*this);//立即执行，不需存储
            delete stmt;//释放空间
        }
        exit(0);
    }
    if (token == "PRINT") {
        Statement *stmt;
        return stmt = new PRINT(parseExp(scanner));
    }
    if (token == "END") {//退出具有特殊性，需特判
        Statement *stmt = new END;
        return stmt;
    }
    if (token == "LET") {
        Statement *stmt;
        std::string var = scanner.nextToken(), tmp = scanner.nextToken();
        if (judgeKey(var)) { error("SYNTAX ERROR"); }
        return stmt = new LET(var, parseExp(scanner));
    }
    if (token == "INPUT") {
        Statement *stmt;
        std::string var = scanner.nextToken();
        if (scanner.hasMoreTokens()) {
            error("SYNTAX ERROR");
        } else {
            extern std::queue<std::string> lineRead;
            int value;
            std::string input, tmp;
            stmt = new INPUT(var);
            while (1) {
                //一直读入直至找到正确输入,需将输入有关语句存到input，其他返回主程序
                getline(std::cin, input);//////为什么std::cin >> input 后主程序无法读入？
                if (input.empty()) { return stmt; }
                TokenScanner scannerTmp;
                scannerTmp.ignoreWhitespace();
                scannerTmp.scanNumbers();
                scannerTmp.setInput(input);
                std::string tmpFirst = scannerTmp.nextToken();
                std::string tmpSecond = scannerTmp.nextToken();
                if (judgeKey(tmpFirst) ||
                    (scannerTmp.getTokenType(tmpFirst) == NUMBER && judgeKey(tmpSecond))) {
                    lineRead.push(input);
                    continue;//若不为输入有关语句，将这些读入返回主程序
                }
                inputStream.push(input);//将输入存入
            }
        }
    }
    if (token == "GOTO") {
        Statement *stmt;
        int lineGoto;
        try {
            lineGoto = stringToInteger(scanner.nextToken());
        } catch (...) { error("SYNTAX ERROR"); }
        return stmt = new GOTO(lineGoto);
    }
    if (token == "IF") {
        Statement *stmt;
        std::string tmp, cmp, expStringFirst = "", expStringSecond = "", lineStringGoto;
        while (1) {
            tmp = scanner.nextToken();
            if (tmp == "<" || tmp == ">" || tmp == "=") {
                cmp = tmp;
                break;
            }
            expStringFirst = expStringFirst + " " + tmp;
        }
        while (1) {
            tmp = scanner.nextToken();
            if (tmp == "THEN") {
                break;
            }
            expStringSecond = expStringSecond + " " + tmp;
        }
        lineStringGoto = scanner.nextToken();
        TokenScanner scannerFirst, scannerSecond;
        scannerFirst.ignoreWhitespace();
        scannerFirst.scanNumbers();
        scannerFirst.setInput(expStringFirst);
        scannerSecond.ignoreWhitespace();
        scannerSecond.scanNumbers();
        scannerSecond.setInput(expStringSecond);
        try {
            stmt = new IF(parseExp(scannerFirst), parseExp(scannerSecond),
                          cmp, stringToInteger(lineStringGoto));
        } catch (...) { throw; }
        return stmt;
    }
}

int Program::getFirstLineNumber() {
    if (!lineRef.empty()) {
        return *(lineRef.begin());
    }
    return -1;
}

int Program::getNextLineNumber(int lineNumber) {
    auto iter = lineRef.upper_bound(lineNumber);
    if (iter != lineRef.end()) {
        return *(++iter);
    }
    return -1;
}

bool Program::judgeKey(std::string var) {
    if (var == "REM" || var == "LET" || var == "PRINT" || var == "INPUT" ||
        var == "END" || var == "GOTO" || var == "IF" || var == "RUN" ||
        var == "LIST" || var == "CLEAR" || var == "QUIT" || var == "HELP") {
        return true;
    }
    return false;
}



