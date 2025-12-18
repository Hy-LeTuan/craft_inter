#pragma once

#include <token.hpp>
#include <expr.hpp>

#include <any>

using expr::Expr;

namespace stmt
{
class Visitor;

class Stmt
{
  public:
    Stmt() = default;
    virtual ~Stmt() = 0;

    Stmt(const Stmt&) = delete;
    Stmt& operator=(const Stmt&) = delete;
    Stmt(Stmt&&) = delete;
    Stmt& operator=(Stmt&&) = delete;

    virtual std::any accept(Visitor* visitor) = 0;
};

class Expression : public Stmt
{
  public:
    Expression(Expr* expression);
    ~Expression() override;

    std::any accept(Visitor* visitor) override;

    const Expr* expression;
};

class Print : public Stmt
{
  public:
    Print(Expr* expression);
    ~Print() override;

    std::any accept(Visitor* visitor) override;

    const Expr* expression;
};

class Visitor
{
  public:
    virtual std::any visitExpressionStmt(Expression* stmt) = 0;
    virtual std::any visitPrintStmt(Print* stmt) = 0;
};
}
