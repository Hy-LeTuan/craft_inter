#include <runtime_error.hpp>
#include <environment.hpp>

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
    values.insert({ name, value });
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
