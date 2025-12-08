#include <expr.hpp>
#include <token.hpp>

Binary::Binary(Expr&& left, Token&& op, Expr&& right)
  : Expr()
  , left{ std::make_unique<Expr>(std::move(left)) }
  , op{ std::make_unique<Token>(std::move(op)) }
  , right{ std::make_unique<Expr>(std::move(right)) }
{
}

Grouping::Grouping(Expr&& expression)
  : Expr()
  , expression{ std::make_unique<Expr>(std::move(expression)) }
{
}

Literal::Literal(LiteralValue&& value)
  : Expr()
  , value{ std::make_unique<LiteralValue>(std::move(value)) }
{
}

Unary::Unary(Token&& op, Expr&& right)
  : Expr()
  , op{ std::make_unique<Token>(std::move(op)) }
  , right{ std::make_unique<Expr>(std::move(right)) }
{
}
