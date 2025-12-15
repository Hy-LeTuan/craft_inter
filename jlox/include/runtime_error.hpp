#pragma once

#include <token.hpp>
#include <string>
#include <exception>

class RuntimeError : public std::exception
{
  public:
    explicit RuntimeError(const Token* token, std::string message);
    const char* what() const noexcept override;
    std::string getMessage() const noexcept;
    const Token* getToken() const noexcept;

  private:
    std::string message;
    const Token* token;
};
