#include <lox_class.hpp>
#include <lox_instance.hpp>

LoxClass::LoxClass(std::string name, std::unordered_map<std::string, LoxFunction*> methods)
  : name{ name }
  , methods{ methods }
{
}

std::string LoxClass::toString()
{
    return this->name;
}

Object LoxClass::call(Interpreter* interpreter, std::vector<Object>* arguments)
{
    LoxInstance* instance = new LoxInstance(this);
    LoxFunction* initializer = findMethod("init");

    if (initializer)
    {
        initializer->bind(instance)->call(interpreter, std::move(arguments));
    }

    return instance;
}

LoxFunction* LoxClass::findMethod(std::string name)
{
    if (methods.count(name))
    {
        return methods.at(name);
    }

    return nullptr;
}

int LoxClass::arity()
{
    LoxFunction* initializer = findMethod("init");

    if (!initializer)
    {
        return 0;
    }

    return initializer->arity();
}
