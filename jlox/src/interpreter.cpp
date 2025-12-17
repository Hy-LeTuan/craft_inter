#include "object.hpp"
#include <runtime_error.hpp>
#include <interpreter.hpp>
#include <iostream>
#include <token_type.hpp>
#include <lox.hpp>

Object Interpreter::visitLiteralExpr(const Literal* expr)
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
    else if (expr->value->index() == 2)
    {
        return std::get<bool>(*expr->value);
    }
    else
    {
        return nullptr;
    }
}

Object Interpreter::visitGroupingExpr(const Grouping* expr)
{
    return evaluate(expr->expression);
}

Object Interpreter::visitUnaryExpr(const Unary* expr)
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

Object Interpreter::visitBinaryExpr(const Binary* expr)
{
    Object left = evaluate(expr->left);
    Object right = evaluate(expr->right);

    switch (expr->op->getType())
    {
        case TokenType::GREATER:
            checkNumberOperand(expr->op, right);
            return ObjectGetDouble(left) > ObjectGetDouble(right);
        case TokenType::GREATER_EQUAL:
            checkNumberOperand(expr->op, right);
            return ObjectGetDouble(left) >= ObjectGetDouble(right);
        case TokenType::LESS:
            checkNumberOperand(expr->op, right);
            return ObjectGetDouble(left) < ObjectGetDouble(right);
        case TokenType::LESS_EQUAL:
            checkNumberOperand(expr->op, right);
            return ObjectGetDouble(left) <= ObjectGetDouble(right);
        case TokenType::BANG_EQUAL:
            return !isEqual(left, right);
        case TokenType::EQUAL_EQUAL:
            return isEqual(left, right);
        case TokenType::MINUS:
            checkNumberOperand(expr->op, right);
            return ObjectGetDouble(left) - ObjectGetDouble(right);
        case TokenType::SLASH:
            checkNumberOperand(expr->op, right);
            return ObjectGetDouble(left) / ObjectGetDouble(right);
        case TokenType::STAR:
            checkNumberOperand(expr->op, right);
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

            throw RuntimeError(expr->op, "Operands must be two numbers or two strings.");
        default:
            return nullptr;
    }

    return nullptr;
}

Object Interpreter::evaluate(const Expr* expr)
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

std::string Interpreter::stringify(Object object)
{
    if (object.type() == typeid(nullptr))
    {
        return "nil";
    }
    else if (object.type() == typeid(double))
    {
        return std::to_string(std::any_cast<double>(object));
    }
    else if (object.type() == typeid(bool))
    {
        bool b = std::any_cast<bool>(object);
        return b ? "true" : "false";
    }
    else if (object.type() == typeid(std::string))
    {
        return std::any_cast<std::string>(object);
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

void checkNumberOperands(const Token* op, Object& left, Object& right)
{
    if (ObjectParser::isDouble(left) && ObjectParser::isDouble(right))
    {
        return;
    }

    throw RuntimeError(op, "Operands must be numbers.");
}

void Interpreter::interpret(Expr* expression)
{
    try
    {
        Object value = evaluate(expression);
        std::cout << stringify(value) << std::endl;
    }
    catch (RuntimeError error)
    {
        Lox::runtimeError(error);
    }
}
