#pragma once

#include <token.hpp>
#include <memory>
#include <any>

class Visitor;

class Expr
{
  public:
    Expr() = default;
    std::any virtual accept(Visitor& visitor);
};

class Binary : public Expr
{
  public:
    Binary(Expr&& left, Token&& op, Expr&& right);
    std::any accept(Visitor& visitor) override;

  private:
    const std::unique_ptr<Expr> left;
    const std::unique_ptr<Token> op;
    const std::unique_ptr<Expr> right;
};

class Grouping : public Expr
{
  public:
    Grouping(Expr&& expression);
    std::any accept(Visitor& visitor) override;

  private:
    const std::unique_ptr<Expr> expression;
};

class Literal : public Expr
{
  public:
    Literal(LiteralValue&& value);
    std::any accept(Visitor& visitor) override;

  private:
    const std::unique_ptr<LiteralValue> value;
};

class Unary : public Expr
{
  public:
    Unary(Token&& op, Expr&& right);
    std::any accept(Visitor& visitor) override;

  private:
    const std::unique_ptr<Token> op;
    const std::unique_ptr<Expr> right;
};

class Visitor
{
  public:
    std::any visitBinaryExpr(Binary* expr);
    std::any visitGroupingExpr(Grouping* expr);
    std::any visitLiteralExpr(Literal* expr);
    std::any visitUnaryExpr(Unary* expr);
};
