#pragma once
#include <token.hpp>
#include <memory>

class Expr
{
  public:
    Expr() = default;
};

class Binary : public Expr
{
  public:
    Binary(Expr&& left, Token&& op, Expr&& right);

  private:
    const std::unique_ptr<Expr> left;
    const std::unique_ptr<Token> op;
    const std::unique_ptr<Expr> right;
};

class Grouping : public Expr
{
  public:
    Grouping(Expr&& expression);

  private:
    const std::unique_ptr<Expr> expression;
};

class Literal : public Expr
{
  public:
    Literal(LiteralValue&& value);

  private:
    const std::unique_ptr<LiteralValue> value;
};

class Unary : public Expr
{
  public:
    Unary(Token&& op, Expr&& right);

  private:
    const std::unique_ptr<Token> op;
    const std::unique_ptr<Expr> right;
};
