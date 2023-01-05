/*
 * File: statement.h
 * -----------------
 * This file defines the Statement abstract type.  In
 * the finished version, this file will also specify subclasses
 * for each of the statement types.  As you design your own
 * version of this class, you should pay careful attention to
 * the exp.h interface, which is an excellent model for
 * the Statement class hierarchy.
 */

#ifndef _statement_h
#define _statement_h

#include <string>
#include <sstream>
#include "evalstate.hpp"
#include "exp.hpp"
#include "Utils/tokenScanner.hpp"
#include "program.hpp"
#include "parser.hpp"
#include "Utils/error.hpp"
#include "Utils/strlib.hpp"

class Program;

/*
 * Class: Statement
 * ----------------
 * This class is used to represent a statement in a program.
 * The model for this class is Expression in the exp.h interface.
 * Like Expression, Statement is an abstract class with subclasses
 * for each of the statement and command types required for the
 * BASIC interpreter.
 */

class Statement {

public:

/*
 * Constructor: Statement
 * ----------------------
 * The base class constructor is empty.  Each subclass must provide
 * its own constructor.
 */

    Statement();

/*
 * Destructor: ~Statement
 * Usage: delete stmt;
 * -------------------
 * The destructor deallocates the storage for this expression.
 * It must be declared virtual to ensure that the correct subclass
 * destructor is called when deleting a statement.
 */

    virtual ~Statement();

/*
 * Method: execute
 * Usage: stmt->execute(state);
 * ----------------------------
 * This method executes a BASIC statement.  Each of the subclasses
 * defines its own execute method that implements the necessary
 * operations.  As was true for the expression evaluator, this
 * method takes an EvalState object for looking up variables or
 * controlling the operation of the interpreter.
 */

    virtual void execute(Program &program) = 0;

};

/*
 * The remainder of this file must consists of subclass
 * definitions for the individual statement forms.  Each of
 * those subclasses must define a constructor that parses a
 * statement from a scanner and a method called execute,
 * which executes that statement.  If the private data for
 * a subclass includes data allocated on the heap (such as
 * an Expression object), the class implementation must also
 * specify its own destructor method to free that memory.
 */

class REM : public Statement {
public:
    virtual void execute(Program &program) override;
};

class LIST : public Statement {
public:
    virtual void execute(Program &program) override;
};

class RUN : public Statement {
public:
    virtual void execute(Program &program) override;
};

class CLEAR : public Statement {
public:
    virtual void execute(Program &program) override;
};

class PRINT : public Statement {
public:
    PRINT(Expression *expTmp) : exp(expTmp) {}

    virtual ~PRINT() { delete exp; }

    virtual void execute(Program &program) override;

private:
    Expression *exp;
};

class END : public RUN {
public:
    virtual void execute(Program &program) override;
};

class LET : public Statement {
public:
    virtual void execute(Program &program) override;

    LET(std::string varTmp, Expression *expTmp) :
            var(varTmp), exp(expTmp) {}

    virtual ~LET() { delete exp; }

private:
    std::string var;
    Expression *exp;
};


class INPUT : public Statement {
public:
    virtual void execute(Program &program) override;

    INPUT(std::string varTmp) : var(varTmp) {}

private:
    std::string var;
};

class GOTO : public Statement {
public:
    virtual void execute(Program &program) override;

    GOTO(int lineGoto) : lineTo(lineGoto) {}

private:
    int lineTo;
};

class IF : public Statement {
public:
    virtual void execute(Program &program) override;

    IF(Expression *expFirstTmp, Expression *expSecondTmp,
       std::string cmpTmp, int lineToTmp) :
            expFirst(expFirstTmp), expSecond(expSecondTmp),
            cmp(cmpTmp), lineTo(lineToTmp) {}

    ~IF() {
        delete expFirst;
        delete expSecond;
    }

private:
    Expression *expFirst, *expSecond;
    std::string cmp;
    int lineTo;
};

#endif
