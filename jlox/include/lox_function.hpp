#pragma once

#include <lox_callable.hpp>

class LoxFunction : public LoxCallable
{
  public:
    LoxFunction(const stmt::Function*);
    ~LoxFunction();
    Object call(Interpreter* interpreter, std::vector<Object>* arguments) override;
    int arity() override;
    std::string toString() override;

  private:
    const stmt::Function* declaration;
};
