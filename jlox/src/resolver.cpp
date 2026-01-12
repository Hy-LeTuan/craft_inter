#include <lox.hpp>
#include <resolver.hpp>

#include <iostream>

Resolver::Resolver(Interpreter* interpreter)
  : interpreter{ interpreter }
{
}

Object Resolver::visitExpressionStmt(const stmt::Expression* stmt)
{
    resolve(stmt->expression);
    return nullptr;
}

Object Resolver::visitIfStmt(const stmt::If* stmt)
{
    resolve(stmt->condition);
    resolve(stmt->thenBranch);

    if (stmt->elseBranch)
    {
        resolve(stmt->elseBranch);
    }

    return nullptr;
}

Object Resolver::visitFunctionStmt(const stmt::Function* stmt)
{
    declare(stmt->name);
    define(stmt->name);

    resolveFunction(stmt, FunctionType::FUNCTION);

    std::cout << "complete function statement" << std::endl;

    return nullptr;
}

Object Resolver::visitBlockStmt(const stmt::Block* stmt)
{
    beginScope();
    resolve(stmt->statements);
    endScope();

    return nullptr;
}

Object Resolver::visitClassStmt(const stmt::Class* stmt)
{
    declare(stmt->name);
    define(stmt->name);

    return nullptr;
}

Object Resolver::visitPrintStmt(const stmt::Print* stmt)
{
    resolve(stmt->expression);

    return nullptr;
}

Object Resolver::visitReturnStmt(const stmt::Return* stmt)
{
    switch (currentFunction)
    {
        case FunctionType::NONE:
            Lox::error(stmt->keyword->clone(), "Can't return from top-level code.");
            break;
        case FunctionType::FUNCTION:
            if (stmt->value)
            {
                resolve(stmt->value);
            }
            break;
    }

    return nullptr;
}

Object Resolver::visitVarStmt(const stmt::Var* stmt)
{
    declare(stmt->name);

    if (stmt->initializer)
    {
        resolve(stmt->initializer);
    }

    define(stmt->name);

    std::cout << "complete var statement" << std::endl;

    return nullptr;
}

Object Resolver::visitWhileStmt(const stmt::While* stmt)
{
    resolve(stmt->condition);
    resolve(stmt->body);

    return nullptr;
}

Object Resolver::visitBinaryExpr(const expr::Binary* expr)
{
    resolve(expr->left);
    resolve(expr->right);

    return nullptr;
}

Object Resolver::visitCallExpr(const expr::Call* expr)
{
    resolve(expr->callee);

    for (auto arg : *expr->arguments)
    {
        resolve(arg);
    }

    return nullptr;
}

Object Resolver::visitGetExpr(const expr::Get* expr)
{
    resolve(expr->object);
    return nullptr;
}

Object Resolver::visitAssignExpr(const expr::Assign* expr)
{
    resolve(expr->value);
    resolveLocal(expr, expr->name);

    return nullptr;
}

Object Resolver::visitGroupingExpr(const expr::Grouping* expr)
{
    resolve(expr->expression);

    return nullptr;
}

Object Resolver::visitLiteralExpr(const expr::Literal* expr)
{
    return nullptr;
}

Object Resolver::visitLogicalExpr(const expr::Logical* expr)
{
    resolve(expr->left);
    resolve(expr->right);

    return nullptr;
}

Object Resolver::visitUnaryExpr(const expr::Unary* expr)
{
    resolve(expr->right);

    return nullptr;
}

Object Resolver::visitVariableExpr(const expr::Variable* expr)
{
    if (scopes.empty())
    {
        return nullptr;
    }

    // check if a variable is being refered in its own initialization
    if (PEEK(scopes).count(LEXEME(expr->name)) && !PEEK(scopes).at(LEXEME(expr->name)))
    {
        Lox::error(expr->name->clone(), "Can't read local variable in its own initializer");
    }

    resolveLocal(expr, expr->name);

    return nullptr;
}

void Resolver::beginScope()
{
    scopes.push_front(std::unordered_map<std::string, bool>{});
}

void Resolver::resolve(const VecStmt* statements)
{
    for (auto statement : *statements)
    {
        resolve(statement);
    }
}

void Resolver::resolve(const stmt::Stmt* statement)
{
    statement->accept(this);
}

void Resolver::resolve(const expr::Expr* expression)
{
    expression->accept(this);
}

void Resolver::endScope()
{
    scopes.pop_front();
}

void Resolver::declare(const Token* name)
{
    if (scopes.empty())
    {
        return;
    }

    auto& scope = PEEK(scopes);

    if (scope.find(LEXEME(name)) != scope.end())
    {
        Lox::error(name->clone(), "Already a variable with this name in this scope.");
    }

    scope.insert_or_assign(name->getLexeme(), false);
}

void Resolver::define(const Token* name)
{
    if (scopes.empty())
    {
        return;
    }

    PEEK(scopes).insert_or_assign(name->getLexeme(), true);
}

void Resolver::resolveLocal(const expr::Expr* expression, const Token* name)
{
    auto temp_stack = scopes;

    int i = scopes.size() - 1;

    for (auto& scope : scopes)
    {
        if (scope.find(LEXEME(name)) != scope.end())
        {
            interpreter->resolve(expression, scopes.size() - 1 - i);
            return;
        }

        i--;
    }
}

void Resolver::resolveFunction(const stmt::Function* stmt, FunctionType type)
{
    FunctionType enclosingFunction = currentFunction;
    currentFunction = type;

    beginScope();

    for (auto param : *stmt->params)
    {
        declare(param);
        define(param);
    }

    std::cout << "finish declaring param" << std::endl;

    resolve(stmt->body);

    endScope();

    currentFunction = enclosingFunction;
}
