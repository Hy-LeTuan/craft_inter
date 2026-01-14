#pragma once

#include <lox_callable.hpp>
#include <chrono>

struct ClockCallable : public LoxCallable
{
    Object call(Interpreter* interpreter, std::vector<Object>* arguments) override
    {
        auto t = std::chrono::system_clock::now();
        double milliseconds =
          std::chrono::duration_cast<std::chrono::milliseconds>(t.time_since_epoch()).count() /
          1000.0;

        return milliseconds;
    }

    int arity() override
    {
        return 0;
    }

    std::string toString() override
    {
        return "<native fn>";
    }
};
