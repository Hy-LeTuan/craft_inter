#pragma once

#include <variant>
#include <string>

using Object = std::variant<std::monostate, double, std::string, bool>;

class ObjectValueVisitor
{
  public:
    std::string operator()(const std::monostate&)
    {
        return "Nil";
    }

    std::string operator()(double d)
    {
        return std::to_string(d);
    }

    std::string operator()(const std::string& s)
    {
        return std::string{ s.begin(), s.end() };
    }

    std::string operator()(bool b)
    {
        return b ? "True" : "False";
    }
};
