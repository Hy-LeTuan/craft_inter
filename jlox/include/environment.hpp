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
    Object getAt(int distance, std::string name);

    void assign(const Token* name, Object value);
    void assignAt(int distance, const Token* name, Object value);

    Environment* getEnclosing();

#if ENVIRONMENT_DEBUG
    void display(std::string name);
#endif

  private:
    Environment* ancestor(int distance);
    std::unordered_map<std::string, Object> values;
    Environment* enclosing;
};
