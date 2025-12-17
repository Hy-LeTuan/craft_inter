#pragma once

#include <any>
#include <string>

using Object = std::any;

#define ObjectGetDouble(val) std::any_cast<double>(val)
#define ObjectGetBool(val) std::any_cast<bool>(val)
#define ObjectGetString(val) std::any_cast<std::string>(val)

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
};
