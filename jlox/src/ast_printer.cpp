#include <ast_printer.hpp>

std::string AstPrinter::print(expr::Expr* expr)
{
    return std::any_cast<std::string>(expr->accept(this));
}

std::any AstPrinter::visitBinaryExpr(const expr::Binary* expr)
{
    return parenthesize(expr->op->getLexeme(), expr->left, expr->right);
}

std::any AstPrinter::visitGroupingExpr(const expr::Grouping* expr)
{
    return parenthesize("group", expr->expression);
}

std::any AstPrinter::visitLiteralExpr(const expr::Literal* expr)
{
    return Token::displayLiteral(*expr->value);
}

std::any AstPrinter::visitUnaryExpr(const expr::Unary* expr)
{
    return parenthesize(expr->op->getLexeme(), expr->right);
}
