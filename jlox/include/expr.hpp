#pragma once

#include <token.hpp>

#include <any>
#include <vector>

using std::vector;

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

class Call : public Expr
{
  public:
    Call(Expr* callee, Token* paren, vector<Expr*>* arguments);
    ~Call() override;

    std::any accept(Visitor* visitor) const override;

    const Expr* callee;
    const Token* paren;
    const vector<Expr*>* arguments;
};

class Get : public Expr
{
  public:
    Get(Expr* object, Token* name);
    ~Get() override;

    std::any accept(Visitor* visitor) const override;

    const Expr* object;
    const Token* name;
};

class Assign : public Expr
{
  public:
    Assign(Token* name, Expr* value);
    ~Assign() override;

    std::any accept(Visitor* visitor) const override;

    const Token* name;
    const Expr* value;
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

class Logical : public Expr
{
  public:
    Logical(Expr* left, Token* op, Expr* right);
    ~Logical() override;

    std::any accept(Visitor* visitor) const override;

    const Expr* left;
    const Token* op;
    const Expr* right;
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

class Variable : public Expr
{
  public:
    Variable(Token* name);
    ~Variable() override;

    std::any accept(Visitor* visitor) const override;

    const Token* name;
};

class Visitor
{
  public:
    virtual std::any visitBinaryExpr(const Binary* expr) = 0;
    virtual std::any visitCallExpr(const Call* expr) = 0;
    virtual std::any visitGetExpr(const Get* expr) = 0;
    virtual std::any visitAssignExpr(const Assign* expr) = 0;
    virtual std::any visitGroupingExpr(const Grouping* expr) = 0;
    virtual std::any visitLiteralExpr(const Literal* expr) = 0;
    virtual std::any visitLogicalExpr(const Logical* expr) = 0;
    virtual std::any visitUnaryExpr(const Unary* expr) = 0;
    virtual std::any visitVariableExpr(const Variable* expr) = 0;
};
}
