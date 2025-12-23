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
