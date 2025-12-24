#include <stmt.hpp>
#include <token.hpp>

#include <any>

namespace stmt
{

Stmt::~Stmt()
{
}

Expression::Expression(Expr* expression)
  : expression{ expression }
{
}

Expression::~Expression()
{
    if (expression)
    {
        delete expression;
    }
}

std::any Expression::accept(Visitor* visitor) const
{
    return visitor->visitExpressionStmt(this);
}

If::If(Expr* condition, Stmt* thenBranch, Stmt* elseBranch)
  : condition{ condition }
  , thenBranch{ thenBranch }
  , elseBranch{ elseBranch }
{
}

If::~If()
{
    if (condition)
    {
        delete condition;
    }
    if (thenBranch)
    {
        delete thenBranch;
    }
    if (elseBranch)
    {
        delete elseBranch;
    }
}

std::any If::accept(Visitor* visitor) const
{
    return visitor->visitIfStmt(this);
}

Block::Block(vector<Stmt*>* statements)
  : statements{ statements }
{
}

Block::~Block()
{
    if (statements)
    {
        delete statements;
    }
}

std::any Block::accept(Visitor* visitor) const
{
    return visitor->visitBlockStmt(this);
}

Print::Print(Expr* expression)
  : expression{ expression }
{
}

Print::~Print()
{
    if (expression)
    {
        delete expression;
    }
}

std::any Print::accept(Visitor* visitor) const
{
    return visitor->visitPrintStmt(this);
}

Var::Var(Token* name, Expr* initializer)
  : name{ name }
  , initializer{ initializer }
{
}

Var::~Var()
{
    if (name)
    {
        delete name;
    }
    if (initializer)
    {
        delete initializer;
    }
}

std::any Var::accept(Visitor* visitor) const
{
    return visitor->visitVarStmt(this);
}

}
