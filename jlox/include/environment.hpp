#pragma once

#include <alias.hpp>
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

#if ENVIRONMENT_DEBUG
    void display(std::string name);
#endif

  private:
    std::unordered_map<std::string, Object> values;
    Environment* enclosing;
};
