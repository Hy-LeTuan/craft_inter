#include <any>
#include <expr.hpp>
#include <token.hpp>

Binary::Binary(Expr&& left, Token&& op, Expr&& right)
  : Expr()
  , left{ std::make_unique<Expr>(std::move(left)) }
  , op{ std::make_unique<Token>(std::move(op)) }
  , right{ std::make_unique<Expr>(std::move(right)) }
{
}

std::any Binary::accept(Visitor& visitor)
{
    return visitor.visitBinaryExpr(this);
}

Grouping::Grouping(Expr&& expression)
  : Expr()
  , expression{ std::make_unique<Expr>(std::move(expression)) }
{
}

std::any Grouping::accept(Visitor& visitor)
{
    return visitor.visitGroupingExpr(this);
}

Literal::Literal(LiteralValue&& value)
  : Expr()
  , value{ std::make_unique<LiteralValue>(std::move(value)) }
{
}

std::any Literal::accept(Visitor& visitor)
{
    return visitor.visitLiteralExpr(this);
}

Unary::Unary(Token&& op, Expr&& right)
  : Expr()
  , op{ std::make_unique<Token>(std::move(op)) }
  , right{ std::make_unique<Expr>(std::move(right)) }
{
}

std::any Unary::accept(Visitor& visitor)
{
    return visitor.visitUnaryExpr(this);
}
