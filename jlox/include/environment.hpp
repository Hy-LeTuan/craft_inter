#pragma once

#include <object.hpp>
#include <token.hpp>

#include <unordered_map>

class Environment
{
  public:
    Environment();
    Environment(Environment* enclosing);
    void define(std::string name, Object value);
    Object get(const Token* name) const;
    void assign(const Token* name, Object value);

  private:
    std::unordered_map<std::string, Object> values;
    Environment* enclosing;
};
