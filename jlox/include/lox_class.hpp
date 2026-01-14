#pragma once

#include <lox_callable.hpp>
#include <lox_function.hpp>

#include <string>

class LoxClass : public LoxCallable
{
  public:
    LoxClass(std::string name, LoxClass* superclass,
      std::unordered_map<std::string, LoxFunction*> methods);

    std::string toString() override;
    Object call(Interpreter* interpreter, std::vector<Object>* arguments) override;
    int arity() override;

    LoxFunction* findMethod(std::string name);

  private:
    std::unordered_map<std::string, LoxFunction*> methods;
    const std::string name;
    LoxClass* superclass;
};
