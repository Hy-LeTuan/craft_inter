#include <object_utils.hpp>

#include <lox_instance.hpp>
#include <lox_callable.hpp>
#include <lox_function.hpp>
#include <lox_class.hpp>

LoxCallable* ObjectParser::GetCallableBase(Object& val)
{
    if (ObjectParser::isCallable(val))
    {
        return ObjectParser::GetCallable(val);
    }
    else if (ObjectParser::isFunction(val))
    {
        return static_cast<LoxCallable*>(ObjectParser::GetFunction(val));
    }
    else if (ObjectParser::isClass(val))
    {
        return static_cast<LoxCallable*>(ObjectParser::GetClass(val));
    }

    return nullptr;
}
