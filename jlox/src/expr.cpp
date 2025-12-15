#include <any>
#include <expr.hpp>
#include <token.hpp>

Expr::~Expr() {

}

Binary::Binary(Expr* left, Token* op, Expr* right)
  : Expr()
  , left{ left }
  , op{ op }
  , right{ right }
{
}

Binary::~Binary()
{
    delete left;
    delete op;
    delete right;
}

std::any Binary::accept(Visitor* visitor) const
{
    return visitor->visitBinaryExpr(this);
}

Grouping::Grouping(Expr* expression)
  : Expr()
  , expression{ expression }
{
}

Grouping::~Grouping()
{
    delete expression;
}

std::any Grouping::accept(Visitor* visitor) const
{
    return visitor->visitGroupingExpr(this);
}

Literal::Literal(Object* value)
  : Expr()
  , value{ value }
{
}

Literal::~Literal()
{
    delete value;
}

std::any Literal::accept(Visitor* visitor) const
{
    return visitor->visitLiteralExpr(this);
}

Unary::Unary(Token* op, Expr* right)
  : Expr()
  , op{ op }
  , right{ right }
{
}

Unary::~Unary()
{
    delete op;
    delete right;
}

std::any Unary::accept(Visitor* visitor) const
{
    return visitor->visitUnaryExpr(this);
}
