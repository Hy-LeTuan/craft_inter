#pragma once

#include <environment.hpp>
#include <lox_callable.hpp>

class LoxInstance;

class LoxFunction : public LoxCallable
{
  public:
    LoxFunction(
      const stmt::Function* declaration, Environment* closure, bool isInitializer = false);
    ~LoxFunction();
    Object call(Interpreter* interpreter, std::vector<Object>* arguments) override;
    int arity() override;
    std::string toString() override;
    LoxFunction* bind(const LoxInstance* instance);

  private:
    const stmt::Function* declaration;
    Environment* closure;
	bool isInitializer;
};
