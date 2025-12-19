#include <stmt.hpp>
#include <token.hpp>

#include <any>
#include <iostream>

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
    std::cout << "destructor for expression statement called" << std::endl;
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
