#include "runtime_error.hpp"
#include <lox_instance.hpp>

LoxInstance::LoxInstance(LoxClass* klass)
  : klass{ klass }
{
}

std::string LoxInstance::toString()
{
    return klass->toString() + " instance";
}

Object LoxInstance::get(const Token* name)
{
    if (fields.count(name->getLexeme()))
    {
        return fields.at(name->getLexeme());
    }

    throw RuntimeError(name, "Undefined property '" + name->getLexeme() + "'.");
}
