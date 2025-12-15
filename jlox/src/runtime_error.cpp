#include <runtime_error.hpp>

RuntimeError::RuntimeError(const Token* token, std::string message)
  : message{ message }
  , token{ token }
{
}

const char* RuntimeError::what() const noexcept
{
    return message.c_str();
}

std::string RuntimeError::getMessage() const noexcept
{
    return message;
}

const Token* RuntimeError::getToken() const noexcept
{
    return token;
}
