#pragma once

#include <object_utils.hpp>
#include <lox_class.hpp>

#include <unordered_map>

class LoxInstance
{
  public:
    LoxInstance(LoxClass* klass);
    Object get(const Token* name);
    std::string toString();

  private:
    LoxClass* klass;
    std::unordered_map<std::string, Object> fields;
};
