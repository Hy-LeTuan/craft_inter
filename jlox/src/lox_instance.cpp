#include <lox_instance.hpp>

#include <runtime_error.hpp>
#include <iostream>

LoxInstance::LoxInstance(LoxClass* klass)
  : klass{ klass }
{
}

std::string LoxInstance::toString()
{
    return klass->toString() + " instance";
}

Object LoxInstance::get(const Token* name) const
{
    if (fields.count(name->getLexeme()))
    {
        return fields.at(name->getLexeme());
    }

    LoxFunction* method = klass->findMethod(name->getLexeme());

    if (method)
    {
        std::cout << "method is bound" << std::endl;
        return method->bind(this);
    }

    throw RuntimeError(name, "Undefined property '" + name->getLexeme() + "'.");
}

void LoxInstance::set(const Token* name, Object value)
{
    fields.insert_or_assign(name->getLexeme(), value);
}
