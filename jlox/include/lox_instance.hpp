#pragma once

#include <lox_class.hpp>

#include <unordered_map>

class LoxInstance
{
  public:
    LoxInstance(LoxClass* klass);
    Object get(const Token* name) const;
    void set(const Token* name, Object value);
    std::string toString();

  private:
    LoxClass* klass;
    std::unordered_map<std::string, Object> fields;
};
