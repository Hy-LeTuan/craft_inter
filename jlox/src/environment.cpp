#include <runtime_error.hpp>
#include <environment.hpp>

#if ENVIRONMENT_DEBUG
#include <iostream>

void Environment::display(std::string name)
{
    std::cout << ">>>>> displaying " + name + " <<<<<" << std::endl;

    for (auto& p : values)
    {
        std::cout << "variable: " << p.first << std::endl;
    }
}
#endif

Environment::Environment()
  : enclosing{ nullptr }
{
}

Environment::Environment(Environment* enclosing)
  : enclosing{ enclosing }
{
}

void Environment::define(std::string name, Object value)
{
    values.insert_or_assign(name, value);
}

Object Environment::get(const Token* name) const
{
    auto it = values.find(name->getLexeme());

    if (it != values.end())
    {
        return it->second;
    }
    else if (enclosing)
    {
        return enclosing->get(name);
    }

    throw RuntimeError{ name, "Undefined variable '" + name->getLexeme() + "'." };
}

Object Environment::getAt(int distance, std::string name)
{
    return ancestor(distance)->values.at(name);
}

void Environment::assign(const Token* name, Object value)
{
    auto it = values.find(name->getLexeme());

    if (it != values.end())
    {
        it->second = value;
        return;
    }
    else if (enclosing)
    {
        enclosing->assign(name, value);
        return;
    }
    else
    {
        throw RuntimeError{ name, "Undefined variable '" + name->getLexeme() + "'." };
    }
}

void Environment::assignAt(int distance, const Token* name, Object value)
{
    ancestor(distance)->values.insert_or_assign(name->getLexeme(), value);
}

Environment* Environment::ancestor(int distance)
{
    Environment* environment = this;

    for (int i = 0; i < distance; i++)
    {
        environment = environment->enclosing;
    }

    return environment;
}
