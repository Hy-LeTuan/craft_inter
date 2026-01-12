#include <alias.hpp>
#include <object_utils.hpp>
#include <ast_printer.hpp>

void AstPrinter::printAll(const VecStmt& statements)
{
    for (auto stmt : statements)
    {
        this->print(stmt);
    }
}

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

Object AstPrinter::visitFunctionStmt(const stmt::Function* stmt)
{
    std::string body;
    body = body + "\n{\n" + stringify_block(stmt->body) + "\n}";

    std::string params = stringify_tokens(stmt->params);

    return "FUNCTION STATEMENT [" + stmt->name->getLexeme() + "] " + "(" + params + ")" + body;
}

Object AstPrinter::visitIfStmt(const stmt::If* stmt)
{
    std::string thenBranch = ObjectGetString(stmt->thenBranch->accept(this));
    std::string condition = ObjectGetString(stmt->condition->accept(this));

    std::string message = "IF STATEMENT [" + condition + "]; then " + thenBranch;

    if (stmt->elseBranch)
    {
        std::string elseBranch = ObjectGetString(stmt->elseBranch->accept(this));
        message = message + "else " + elseBranch;
    }

    return message;
}

Object AstPrinter::visitBlockStmt(const stmt::Block* stmt)
{
    std::string message = "BLOCK STATEMENT: \n{\n";
    std::string block = stringify_block(stmt->statements);

    message += block;
    message += "}";

    return message;
}

Object AstPrinter::visitClassStmt(const stmt::Class* stmt)
{
    std::string name = stmt->name->getLexeme();
    return "CLASS STATEMENT: [" + name + "]";
}

Object AstPrinter::visitPrintStmt(const stmt::Print* stmt)
{
    return "PRINT STATEMENT: " + ObjectGetString(stmt->expression->accept(this));
}

Object AstPrinter::visitReturnStmt(const stmt::Return* stmt)
{
    if (stmt->value)
    {
        return "RETURN STATEMENT: " + ObjectGetString(stmt->value->accept(this));
    }

    return std::string{ "RETURN STATEMENT: null" };
}

Object AstPrinter::visitVarStmt(const stmt::Var* stmt)
{
    std::string initializer = "";

    if (stmt->initializer)
    {
        initializer = "; Initializer: " + ObjectGetString(stmt->initializer->accept(this));
    }

    return "VAR STATEMENT: " + stmt->name->getLexeme() + initializer;
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

Object AstPrinter::visitCallExpr(const expr::Call* expr)
{
    std::string callee = ObjectGetString(expr->callee->accept(this));

    std::string args;

    for (auto arg : *expr->arguments)
    {
        args += ObjectGetString(arg->accept(this));
        args += ", ";
    }

    if (args.length() > 0)
    {
        args = args.substr(0, args.length() - 2);
    }

    return "Function call: [" + callee + "]" + " (" + args + ")";
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
    return "Literal: " + Token::displayLiteral(*expr->value);
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
    return "`VAR: " + expr->name->getLexeme() + "`";
}

std::string AstPrinter::stringify_block(const std::vector<stmt::Stmt*>* statements)
{
    if (statements->size() == 1)
    {
        return "\t" + ObjectGetString(statements->at(0)->accept(this));
    }

    std::string message = "";

    int index = 0;
    for (auto stmt : *statements)
    {
        message += "\t" + ObjectGetString(stmt->accept(this));

        if (index != statements->size() - 1)
        {
            message += "\n";
        }

        index++;
    }

    return message;
}

std::string AstPrinter::stringify_tokens(const std::vector<Token*>* params)
{
    if (params->empty())
    {
        return "";
    }

    std::string params_str = "";

    for (auto param : *params)
    {
        params_str += param->getLexeme();
        params_str += ", ";
    }

    if (params_str.length() > 0)
    {
        params_str = params_str.substr(0, params_str.length() - 2);
    }

    return params_str;
}
