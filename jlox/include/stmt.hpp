#pragma once

#include <token.hpp>
#include <expr.hpp>

#include <any>
#include <vector>

using expr::Expr;
using std::vector;

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

    virtual std::any accept(Visitor* visitor) const = 0;
};

class Expression : public Stmt
{
  public:
    Expression(Expr* expression);
    ~Expression() override;

    std::any accept(Visitor* visitor) const override;

    const Expr* expression;
};

class If : public Stmt
{
  public:
    If(Expr* condition, Stmt* thenBranch, Stmt* elseBranch);
    ~If() override;

    std::any accept(Visitor* visitor) const override;

    const Expr* condition;
    const Stmt* thenBranch;
    const Stmt* elseBranch;
};

class Block : public Stmt
{
  public:
    Block(vector<Stmt*>* statements);
    ~Block() override;

    std::any accept(Visitor* visitor) const override;

    const vector<Stmt*>* statements;
};

class Print : public Stmt
{
  public:
    Print(Expr* expression);
    ~Print() override;

    std::any accept(Visitor* visitor) const override;

    const Expr* expression;
};

class Var : public Stmt
{
  public:
    Var(Token* name, Expr* initializer);
    ~Var() override;

    std::any accept(Visitor* visitor) const override;

    const Token* name;
    const Expr* initializer;
};

class While : public Stmt
{
  public:
    While(Expr* condition, Stmt* body);
    ~While() override;

    std::any accept(Visitor* visitor) const override;

    const Expr* condition;
    const Stmt* body;
};

class Visitor
{
  public:
    virtual std::any visitExpressionStmt(const Expression* stmt) = 0;
    virtual std::any visitIfStmt(const If* stmt) = 0;
    virtual std::any visitBlockStmt(const Block* stmt) = 0;
    virtual std::any visitPrintStmt(const Print* stmt) = 0;
    virtual std::any visitVarStmt(const Var* stmt) = 0;
    virtual std::any visitWhileStmt(const While* stmt) = 0;
};
}
