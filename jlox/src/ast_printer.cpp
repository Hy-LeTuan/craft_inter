#include <ast_printer.hpp>

std::string AstPrinter::print(Expr* expr)
{
    return std::any_cast<std::string>(expr->accept(this));
}

std::any AstPrinter::visitBinaryExpr(const Binary* expr)
{
    return parenthesize(expr->op->getLexeme(), expr->left, expr->right);
}

std::any AstPrinter::visitGroupingExpr(const Grouping* expr)
{
    return parenthesize("group", expr->expression);
}

std::any AstPrinter::visitLiteralExpr(const Literal* expr)
{
    if (expr->value->index() == 0)
    {
        double val = std::get<double>(*(expr->value));
        std::cout << "val: " << val << std::endl;

        return std::to_string(val);
    }
    else
    {
        std::string val = std::get<std::string>(*(expr->value));
        return val;
    }
}

std::any AstPrinter::visitUnaryExpr(const Unary* expr)
{
    return parenthesize(expr->op->getLexeme(), expr->right);
}
