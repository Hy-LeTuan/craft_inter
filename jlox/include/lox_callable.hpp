#pragma once

#include <interpreter.hpp>

#include <alias.hpp>
#include <string>

class LoxCallable
{
  public:
    virtual ~LoxCallable()
    {
    }

    virtual Object call(Interpreter* interpreter, std::vector<Object>* arguments) = 0;
    virtual int arity() = 0;
    virtual std::string toString() = 0;
};
