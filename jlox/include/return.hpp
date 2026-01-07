#pragma once

#include <alias.hpp>

#include <exception>

class Return : public std::exception
{
  public:
    explicit Return(Object value);
    const char* what() const noexcept override;
	Object getValue();

  private:
    Object value;
};
