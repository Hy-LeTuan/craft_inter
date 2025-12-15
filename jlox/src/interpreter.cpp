#include "runtime_error.hpp"
#include <interpreter.hpp>
#include <iostream>
#include <token_type.hpp>
#include <lox.hpp>

std::any Interpreter::visitLiteralExpr(const Literal* expr)
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

std::any Interpreter::visitGroupingExpr(const Grouping* expr)
{
    return evaluate(expr->expression);
}

std::any Interpreter::visitUnaryExpr(const Unary* expr)
{
    std::any right = evaluate(expr->right);

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

std::any Interpreter::visitBinaryExpr(const Binary* expr)
{
    std::any left = evaluate(expr->left);
    std::any right = evaluate(expr->right);

    switch (expr->op->getType())
    {
        case TokenType::GREATER:
            checkNumberOperand(expr->op, right);
            return std::any_cast<double>(left) > std::any_cast<double>(right);
        case TokenType::GREATER_EQUAL:
            checkNumberOperand(expr->op, right);
            return std::any_cast<double>(left) >= std::any_cast<double>(right);
        case TokenType::LESS:
            checkNumberOperand(expr->op, right);
            return std::any_cast<double>(left) < std::any_cast<double>(right);
        case TokenType::LESS_EQUAL:
            checkNumberOperand(expr->op, right);
            return std::any_cast<double>(left) <= std::any_cast<double>(right);
        case TokenType::BANG_EQUAL:
            return !isEqual(left, right);
        case TokenType::EQUAL_EQUAL:
            return isEqual(left, right);
        case TokenType::MINUS:
            checkNumberOperand(expr->op, right);
            return std::any_cast<double>(left) - std::any_cast<double>(right);
        case TokenType::SLASH:
            checkNumberOperand(expr->op, right);
            return std::any_cast<double>(left) / std::any_cast<double>(right);
        case TokenType::STAR:
            checkNumberOperand(expr->op, right);
            return std::any_cast<double>(left) * std::any_cast<double>(right);
        case TokenType::PLUS:
            if (left.type() == typeid(double) && right.type() == typeid(double))
            {
                return std::any_cast<double>(left) + std::any_cast<double>(right);
            }
            else if (left.type() == typeid(std::string) && right.type() == typeid(std::string))
            {
                return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
            }

            throw RuntimeError(expr->op, "Operands must be two numbers or two strings.");
        default:
            return nullptr;
    }

    return nullptr;
}

std::any Interpreter::evaluate(const Expr* expr)
{
    return expr->accept(this);
}

bool Interpreter::isTruthy(std::any& right)
{
    if (right.type() == typeid(nullptr))
    {
        return false;
    }
    else if (right.type() == typeid(bool))
    {
        return std::any_cast<bool>(right);
    }

    return true;
}

bool Interpreter::isEqual(std::any& left, std::any& right)
{
    if (left.type() == typeid(nullptr) && right.type() == typeid(nullptr))
    {
        return true;
    }
    else if (left.type() == typeid(nullptr))
    {
        return false;
    }
    else if (left.type() == typeid(std::string) && right.type() == typeid(std::string))
    {
        return std::any_cast<std::string>(left) == std::any_cast<std::string>(right);
    }
    else if (left.type() == typeid(double) && right.type() == typeid(double))
    {
        return std::any_cast<double>(left) == std::any_cast<double>(right);
    }
    else if (left.type() == typeid(std::string) && right.type() == typeid(double))
    {
        return std::any_cast<std::string>(left) == std::to_string(std::any_cast<double>(right));
    }
    else if (left.type() == typeid(double) && right.type() == typeid(std::string))
    {
        return std::any_cast<std::string>(right) == std::to_string(std::any_cast<double>(left));
    }

    return false;
}

std::string Interpreter::stringify(std::any object)
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

void Interpreter::checkNumberOperand(const Token* op, std::any& operand)
{
    if (operand.type() == typeid(double))
    {
        return;
    }

    throw RuntimeError(op, "Operand must be a number.");
}

void checkNumberOperands(const Token* op, std::any& left, std::any& right)
{
    if (left.type() == typeid(double) && right.type() == typeid(double))
    {
        return;
    }

    throw RuntimeError(op, "Operands must be numbers.");
}

void Interpreter::interpret(Expr* expression)
{
    try
    {
        std::any value = evaluate(expression);
        std::cout << stringify(value) << std::endl;
    }
    catch (RuntimeError error)
    {
        Lox::runtimeError(error);
    }
}
