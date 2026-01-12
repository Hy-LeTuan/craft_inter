#pragma once

#include <lox_callable.hpp>

#include <string>

class LoxClass : public LoxCallable
{
  public:
    LoxClass(std::string name);

    std::string toString() override;
    Object call(Interpreter* interpreter, std::vector<Object>* arguments) override;
    int arity() override;

  private:
    const std::string name;
};
