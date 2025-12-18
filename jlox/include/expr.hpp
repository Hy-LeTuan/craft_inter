#pragma once

#include <token.hpp>
#include <any>

namespace expr
{
class Visitor;

class Expr
{
  public:
    Expr() = default;
    virtual ~Expr() = 0;

    Expr(const Expr&) = delete;
    Expr& operator=(const Expr&) = delete;
    Expr(Expr&&) = delete;
    Expr& operator=(Expr&&) = delete;

    virtual std::any accept(Visitor* visitor) const = 0;
};

class Binary : public Expr
{
  public:
    Binary(Expr* left, Token* op, Expr* right);
    ~Binary() override;

    std::any accept(Visitor* visitor) const override;

    const Expr* left;
    const Token* op;
    const Expr* right;
};

class Grouping : public Expr
{
  public:
    Grouping(Expr* expression);
    ~Grouping() override;

    std::any accept(Visitor* visitor) const override;

    const Expr* expression;
};

class Literal : public Expr
{
  public:
    Literal(LiteralValue* value);
    ~Literal() override;

    std::any accept(Visitor* visitor) const override;

    const LiteralValue* value;
};

class Unary : public Expr
{
  public:
    Unary(Token* op, Expr* right);
    ~Unary() override;

    std::any accept(Visitor* visitor) const override;

    const Token* op;
    const Expr* right;
};

class Visitor
{
  public:
    virtual std::any visitBinaryExpr(const Binary* expr) = 0;
    virtual std::any visitGroupingExpr(const Grouping* expr) = 0;
    virtual std::any visitLiteralExpr(const Literal* expr) = 0;
    virtual std::any visitUnaryExpr(const Unary* expr) = 0;
};
}
