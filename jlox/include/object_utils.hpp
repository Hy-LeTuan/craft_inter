#pragma once

#include "lox_instance.hpp"
#include <alias.hpp>
#include <lox_callable.hpp>
#include <lox_function.hpp>
#include <lox_class.hpp>

#define ObjectGetDouble(val) std::any_cast<double>(val)
#define ObjectGetBool(val) std::any_cast<bool>(val)
#define ObjectGetString(val) std::any_cast<std::string>(val)
#define ObjectGetCallable(val) std::any_cast<LoxCallable*>(val)
#define ObjectGetFunction(val) std::any_cast<LoxFunction*>(val)
#define ObjectGetClass(val) std::any_cast<LoxClass*>(val)
#define ObjectGetInstance(val) std::any_cast<LoxInstance*>(val)

class ObjectParser
{
  public:
    static bool isDouble(Object& val)
    {
        return val.type() == typeid(double);
    }

    static bool isBool(Object& val)
    {
        return val.type() == typeid(bool);
    }

    static bool isString(Object& val)
    {
        return val.type() == typeid(std::string);
    }

    static bool isNull(Object& val)
    {
        return val.type() == typeid(nullptr);
    }

    static bool isCallable(Object& val)
    {
        return val.type() == typeid(LoxCallable*);
    }

    static bool isFunction(Object& val)
    {
        return val.type() == typeid(LoxFunction*);
    }

    static bool isClass(Object& val)
    {
        return val.type() == typeid(LoxClass*);
    }

    static bool isInstance(Object& val)
    {
        return val.type() == typeid(LoxInstance*);
    }
};
