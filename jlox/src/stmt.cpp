#include <stmt.hpp>
#include <token.hpp>

#include <any>

namespace stmt
{
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

std::any Expression::accept(Visitor* visitor)
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

std::any Print::accept(Visitor* visitor)
{
    return visitor->visitPrintStmt(this);
}

}
