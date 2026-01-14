#pragma once

#include <any>
#include <string>

class LoxCallable;
class LoxClass;
class LoxFunction;
class LoxInstance;

using Object = std::any;

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

    static inline double GetDouble(Object val)
    {
        return std::any_cast<double>(val);
    }

    static inline bool GetBool(Object val)
    {
        return std::any_cast<bool>(val);
    }

    static inline std::string GetString(Object val)
    {
        return std::any_cast<std::string>(val);
    }

    static inline LoxCallable* GetCallable(Object val)
    {
        if (ObjectParser::isNull(val))
        {
            return nullptr;
        }

        return std::any_cast<LoxCallable*>(val);
    }

    static inline LoxFunction* GetFunction(Object val)
    {
        if (ObjectParser::isNull(val))
        {
            return nullptr;
        }

        return std::any_cast<LoxFunction*>(val);
    }

    static inline LoxClass* GetClass(Object val)
    {
        if (ObjectParser::isNull(val))
        {
            return nullptr;
        }

        return std::any_cast<LoxClass*>(val);
    }

    static inline LoxInstance* GetInstance(Object val)
    {
        if (ObjectParser::isNull(val))
        {
            return nullptr;
        }

        return std::any_cast<LoxInstance*>(val);
    }

    static inline bool isCallableBase(Object val)
    {
        return ObjectParser::isCallable(val) || ObjectParser::isClass(val) ||
          ObjectParser::isFunction(val);
    }

    static LoxCallable* GetCallableBase(Object& val);
};
