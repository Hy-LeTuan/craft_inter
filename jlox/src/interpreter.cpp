#include "object.hpp"
#include <runtime_error.hpp>
#include <interpreter.hpp>
#include <token_type.hpp>
#include <lox.hpp>
#include <iostream>

Object Interpreter::visitExpressionStmt(const stmt::Expression* stmt)
{
    evaluate(stmt->expression);
    return nullptr;
}

Object Interpreter::visitPrintStmt(const stmt::Print* stmt)
{
    Object value = evaluate(stmt->expression);
    std::cout << stringify(value) << std::endl;

    return nullptr;
}

Object Interpreter::visitVarStmt(const stmt::Var* stmt)
{
    Object value = nullptr;

    if (stmt->initializer != nullptr)
    {
        value = evaluate(stmt->initializer);
    }

    environment.define(stmt->name->getLexeme(), value);
    return nullptr;
}

Object Interpreter::visitVariableExpr(const expr::Variable* expr)
{
    return environment.get(expr->name);
}

Object Interpreter::visitAssignExpr(const expr::Assign* expr)
{
    Object value = evaluate(expr->value);
    environment.assign(expr->name, value);

    return value;
}

Object Interpreter::visitLiteralExpr(const expr::Literal* expr)
{
    if (expr->value->index() == 0)
    {
        return nullptr;
    }
    // double
    else if (expr->value->index() == 1)
    {
        return std::get<double>(*expr->value);
    }
    // string
    else if (expr->value->index() == 2)
    {
        return std::get<std::string>(*expr->value);
    }
    // bool
    else if (expr->value->index() == 3)
    {
        return std::get<bool>(*expr->value);
    }
    else
    {
        return nullptr;
    }
}

Object Interpreter::visitGroupingExpr(const expr::Grouping* expr)
{
    return evaluate(expr->expression);
}

Object Interpreter::visitUnaryExpr(const expr::Unary* expr)
{
    Object right = evaluate(expr->right);

    switch (expr->op->getType())
    {
        case TokenType::MINUS:
            checkNumberOperand(expr->op, right);
            return (std::any_cast<double>(right));
        case TokenType::BANG:
            return !isTruthy(right);
        default:
            return nullptr;
    }
}

Object Interpreter::visitBinaryExpr(const expr::Binary* expr)
{
    Object left = evaluate(expr->left);
    Object right = evaluate(expr->right);

    switch (expr->op->getType())
    {
        case TokenType::GREATER:
            checkNumberOperands(expr->op, left, right);
            return ObjectGetDouble(left) > ObjectGetDouble(right);
        case TokenType::GREATER_EQUAL:
            checkNumberOperands(expr->op, left, right);
            return ObjectGetDouble(left) >= ObjectGetDouble(right);
        case TokenType::LESS:
            checkNumberOperands(expr->op, left, right);
            return ObjectGetDouble(left) < ObjectGetDouble(right);
        case TokenType::LESS_EQUAL:
            checkNumberOperands(expr->op, left, right);
            return ObjectGetDouble(left) <= ObjectGetDouble(right);
        case TokenType::BANG_EQUAL:
            return !isEqual(left, right);
        case TokenType::EQUAL_EQUAL:
            return isEqual(left, right);
        case TokenType::MINUS:
            checkNumberOperands(expr->op, left, right);
            return ObjectGetDouble(left) - ObjectGetDouble(right);
        case TokenType::SLASH:
            checkNumberOperands(expr->op, left, right);
            if (ObjectGetDouble(right) == 0.)
            {
                throw RuntimeError(expr->op, "Divide by zero error.");
            }

            return ObjectGetDouble(left) / ObjectGetDouble(right);
        case TokenType::STAR:
            checkNumberOperands(expr->op, left, right);
            return ObjectGetDouble(left) * ObjectGetDouble(right);
        case TokenType::PLUS:
            if (ObjectParser::isDouble(left) && ObjectParser::isDouble(right))
            {
                return ObjectGetDouble(left) + ObjectGetDouble(right);
            }
            else if (ObjectParser::isString(left) && ObjectParser::isString(right))
            {
                return ObjectGetString(left) + ObjectGetString(right);
            }
            else if (ObjectParser::isString(left) && ObjectParser::isDouble(right))
            {
                return ObjectGetString(left) + std::to_string(ObjectGetDouble(right));
            }

            throw RuntimeError(expr->op, "Operands must be two numbers or two strings.");
        default:
            return nullptr;
    }

    return nullptr;
}

Object Interpreter::evaluate(const expr::Expr* expr)
{
    return expr->accept(this);
}

bool Interpreter::isTruthy(Object& right)
{
    if (ObjectParser::isNull(right))
    {
        return false;
    }
    else if (ObjectParser::isBool(right))
    {
        return ObjectGetBool(right);
    }

    return true;
}

bool Interpreter::isEqual(Object& left, Object& right)
{
    if (ObjectParser::isNull(left) && ObjectParser::isNull(right))
    {
        return true;
    }
    else if (ObjectParser::isNull(left))
    {
        return false;
    }
    else if (ObjectParser::isString(left) && ObjectParser::isString(right))
    {
        return ObjectGetString(left) == ObjectGetString(right);
    }
    else if (ObjectParser::isDouble(left) && ObjectParser::isDouble(right))
    {
        return ObjectGetDouble(left) == ObjectGetDouble(right);
    }
    else if (ObjectParser::isString(left) && ObjectParser::isDouble(right))
    {
        return ObjectGetString(left) == std::to_string(ObjectGetDouble(right));
    }
    else if (ObjectParser::isDouble(left) && ObjectParser::isString(right))
    {
        return ObjectGetString(right) == std::to_string(ObjectGetDouble(left));
    }

    return false;
}

std::string Interpreter::stringify(Object& object)
{
    if (ObjectParser::isNull(object))
    {
        return "nil";
    }
    else if (ObjectParser::isDouble(object))
    {
        return std::to_string(ObjectGetDouble(object));
    }
    else if (ObjectParser::isBool(object))
    {
        bool b = ObjectGetBool(object);
        return b ? "true" : "false";
    }
    else if (ObjectParser::isString(object))
    {
        return ObjectGetString(object);
    }

    return "";
}

void Interpreter::checkNumberOperand(const Token* op, Object& operand)
{
    if (ObjectParser::isDouble(operand))
    {
        return;
    }

    throw RuntimeError(op, "Operand must be a number.");
}

void Interpreter::checkNumberOperands(const Token* op, Object& left, Object& right)
{
    if (ObjectParser::isDouble(left) && ObjectParser::isDouble(right))
    {
        return;
    }

    throw RuntimeError(op, "Operands must be numbers.");
}

void Interpreter::interpret(std::vector<stmt::Stmt*> statements)
{
    try
    {
        for (auto statement : statements)
        {
            execute(statement);
        }
    }
    catch (RuntimeError error)
    {
        Lox::runtimeError(error);
    }
}

void Interpreter::execute(stmt::Stmt* statement)
{
    statement->accept(this);
}
