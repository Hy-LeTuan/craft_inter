#include <lox_class.hpp>
#include <lox_instance.hpp>

LoxClass::LoxClass(std::string name)
  : name{ name }
{
}

std::string LoxClass::toString()
{
    return this->name;
}

Object LoxClass::call(Interpreter* interpreter, std::vector<Object>* arguments)
{
    LoxInstance* instance = new LoxInstance(this);
    return instance;
}

int LoxClass::arity()
{
    return 0;
}
