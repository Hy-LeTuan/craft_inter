#include "object.hpp"
#include <ast_printer.hpp>

void AstPrinter::print(const stmt::Stmt* stmt)
{
    std::cout << stringify(stmt) << std::endl;
}

std::string AstPrinter::stringify(const stmt::Stmt* stmt)
{
    return ObjectGetString(stmt->accept(this));
}

Object AstPrinter::visitExpressionStmt(const stmt::Expression* stmt)
{
    std::string expression = ObjectGetString(stmt->expression->accept(this));
    return "EXPR STATEMENT: " + expression;
}

Object AstPrinter::visitIfStmt(const stmt::If* stmt)
{
    std::string thenBranch = ObjectGetString(stmt->thenBranch->accept(this));
    std::string condition = ObjectGetString(stmt->condition->accept(this));

    std::string message = "IF STATEMENT () " + condition + " then " + thenBranch;

    if (stmt->elseBranch)
    {
        std::string elseBranch = ObjectGetString(stmt->elseBranch->accept(this));
        message = message + "else " + elseBranch;
    }

    return message;
}

Object AstPrinter::visitBlockStmt(const stmt::Block* stmt)
{
    std::string message = "BLOCK STATEMENT: {\n";

    for (auto stmt : *stmt->statements)
    {
        message += "\t" + ObjectGetString(stmt->accept(this));
        message += "\n";
    }

    message += "}";

    return message;
}

Object AstPrinter::visitPrintStmt(const stmt::Print* stmt)
{
    return "PRINT STATEMENT: " + ObjectGetString(stmt->expression->accept(this));
}

Object AstPrinter::visitVarStmt(const stmt::Var* stmt)
{
    return "VAR STATEMENT: " + stmt->name->getLexeme();
}

Object AstPrinter::visitWhileStmt(const stmt::While* stmt)
{
    std::string message = "WHILE STATEMENT: (";
    message += ObjectGetString(stmt->condition->accept(this));
    message += ")\n";

    message += "{\n";
    message += ObjectGetString(stmt->body->accept(this));
    message += "}\n";

    return message;
}

Object AstPrinter::visitBinaryExpr(const expr::Binary* expr)
{
    return parenthesize(expr->op->getLexeme(), expr->left, expr->right);
}

Object AstPrinter::visitAssignExpr(const expr::Assign* expr)
{
    std::string value = ObjectGetString(expr->value->accept(this));
    return "(" + expr->name->getLexeme() + ")" + " <- " + value;
}

Object AstPrinter::visitGroupingExpr(const expr::Grouping* expr)
{
    return parenthesize("group", expr->expression);
}

Object AstPrinter::visitLiteralExpr(const expr::Literal* expr)
{
    return Token::displayLiteral(*expr->value);
}

Object AstPrinter::visitUnaryExpr(const expr::Unary* expr)
{
    return parenthesize(expr->op->getLexeme(), expr->right);
}

Object AstPrinter::visitLogicalExpr(const expr::Logical* expr)
{
    return parenthesize(expr->op->getLexeme(), expr->left, expr->right);
}

Object AstPrinter::visitVariableExpr(const expr::Variable* expr)
{
    return "`" + expr->name->getLexeme() + "`";
}
