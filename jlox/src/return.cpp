#include <return.hpp>

Return::Return(Object value)
  : value{ value }
{
}

const char* Return::what() const noexcept
{
    return "Return exception";
}

Object Return::getValue()
{
    return this->value;
}
