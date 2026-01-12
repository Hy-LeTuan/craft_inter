#include <expr.hpp>
#include <token.hpp>

#include <any>

namespace expr
{

Expr::~Expr()
{
}

Binary::Binary(Expr* left, Token* op, Expr* right)
  : left{ left }
  , op{ op }
  , right{ right }
{
}

Binary::~Binary()
{
    if (left)
    {
        delete left;
    }
    if (op)
    {
        delete op;
    }
    if (right)
    {
        delete right;
    }
}

std::any Binary::accept(Visitor* visitor) const
{
    return visitor->visitBinaryExpr(this);
}

Call::Call(Expr* callee, Token* paren, vector<Expr*>* arguments)
  : callee{ callee }
  , paren{ paren }
  , arguments{ arguments }
{
}

Call::~Call()
{
    if (callee)
    {
        delete callee;
    }
    if (paren)
    {
        delete paren;
    }
    if (arguments)
    {
        delete arguments;
    }
}

std::any Call::accept(Visitor* visitor) const
{
    return visitor->visitCallExpr(this);
}

Get::Get(Expr* object, Token* name)
  : object{ object }
  , name{ name }
{
}

Get::~Get()
{
    if (object)
    {
        delete object;
    }
    if (name)
    {
        delete name;
    }
}

std::any Get::accept(Visitor* visitor) const
{
    return visitor->visitGetExpr(this);
}

Assign::Assign(Token* name, Expr* value)
  : name{ name }
  , value{ value }
{
}

Assign::~Assign()
{
    if (name)
    {
        delete name;
    }
    if (value)
    {
        delete value;
    }
}

std::any Assign::accept(Visitor* visitor) const
{
    return visitor->visitAssignExpr(this);
}

Grouping::Grouping(Expr* expression)
  : expression{ expression }
{
}

Grouping::~Grouping()
{
    if (expression)
    {
        delete expression;
    }
}

std::any Grouping::accept(Visitor* visitor) const
{
    return visitor->visitGroupingExpr(this);
}

Literal::Literal(LiteralValue* value)
  : value{ value }
{
}

Literal::~Literal()
{
    if (value)
    {
        delete value;
    }
}

std::any Literal::accept(Visitor* visitor) const
{
    return visitor->visitLiteralExpr(this);
}

Logical::Logical(Expr* left, Token* op, Expr* right)
  : left{ left }
  , op{ op }
  , right{ right }
{
}

Logical::~Logical()
{
    if (left)
    {
        delete left;
    }
    if (op)
    {
        delete op;
    }
    if (right)
    {
        delete right;
    }
}

std::any Logical::accept(Visitor* visitor) const
{
    return visitor->visitLogicalExpr(this);
}

Unary::Unary(Token* op, Expr* right)
  : op{ op }
  , right{ right }
{
}

Unary::~Unary()
{
    if (op)
    {
        delete op;
    }
    if (right)
    {
        delete right;
    }
}

std::any Unary::accept(Visitor* visitor) const
{
    return visitor->visitUnaryExpr(this);
}

Variable::Variable(Token* name)
  : name{ name }
{
}

Variable::~Variable()
{
    if (name)
    {
        delete name;
    }
}

std::any Variable::accept(Visitor* visitor) const
{
    return visitor->visitVariableExpr(this);
}

}
