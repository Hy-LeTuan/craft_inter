#include <interpreter.hpp>

#include <runtime_error.hpp>
#include <return.hpp>

#include <object_utils.hpp>
#include <token_type.hpp>
#include <ast_printer.hpp>

#include <lox_callable.hpp>
#include <lox_function.hpp>

#include <lox.hpp>

#include <iomanip>
#include <ios>
#include <string>
#include <chrono>

struct ClockCallable : public LoxCallable
{
    Object call(Interpreter* interpreter, std::vector<Object>* arguments) override
    {
        auto t = std::chrono::system_clock::now();
        double milliseconds =
          std::chrono::duration_cast<std::chrono::milliseconds>(t.time_since_epoch()).count() /
          1000.0;

        return milliseconds;
    }

    int arity() override
    {
        return 0;
    }

    std::string toString() override
    {
        return "<native fn>";
    }
};

Interpreter::Interpreter()
{
    globals = new Environment();
    environment = globals;

    globals->define("clock", static_cast<LoxCallable*>(new ClockCallable()));
}

Interpreter::~Interpreter()
{
    delete environment;
}

Object Interpreter::visitWhileStmt(const stmt::While* stmt)
{
    while (isTruthy(evaluate(stmt->condition)))
    {
        execute(stmt->body);
    }

    return nullptr;
}

Object Interpreter::visitIfStmt(const stmt::If* stmt)
{
    Object condition = evaluate(stmt->condition);

    if (isTruthy(condition))
    {
        execute(stmt->thenBranch);
    }
    else if (stmt->elseBranch)
    {
        execute(stmt->elseBranch);
    }

    return nullptr;
}

Object Interpreter::visitFunctionStmt(const stmt::Function* stmt)
{
    LoxFunction* function = new LoxFunction(stmt);
    environment->define(stmt->name->getLexeme(), static_cast<LoxCallable*>(function));

    return nullptr;
}

Object Interpreter::visitBlockStmt(const stmt::Block* stmt)
{

    executeBlock(stmt->statements, new Environment{ environment });

    return nullptr;
}

void Interpreter::executeBlock(
  const vector<stmt::Stmt*>* statements, Environment* child_environment)
{
    Environment* previous = this->environment;

    try
    {
        this->environment = child_environment;

        for (auto stmt : *statements)
        {
            execute(stmt);
        }
    }
    catch (...)
    {
        this->environment = previous;
        delete child_environment;

        throw;
    }

    this->environment = previous;
    delete child_environment;
}

Object Interpreter::visitExpressionStmt(const stmt::Expression* stmt)
{
    evaluate(stmt->expression);
    return nullptr;
}

Object Interpreter::visitPrintStmt(const stmt::Print* stmt)
{
    Object value = evaluate(stmt->expression);

    if (ObjectParser::isDouble(value))
    {
        std::cout << std::fixed << std::setprecision(2) << ObjectGetDouble(value) << std::endl;
    }
    else
    {
        std::cout << stringify(value) << std::endl;
    }

    return nullptr;
}

Object Interpreter::visitReturnStmt(const stmt::Return* stmt)
{
    Object value = nullptr;

    if (stmt->value)
    {
        value = evaluate(stmt->value);
    }

    throw Return{ value };
}

Object Interpreter::visitVarStmt(const stmt::Var* stmt)
{
    Object value = nullptr;

    if (stmt->initializer)
    {
        value = evaluate(stmt->initializer);
    }

    environment->define(stmt->name->getLexeme(), value);

    return nullptr;
}

Object Interpreter::visitLogicalExpr(const expr::Logical* expr)
{
    Object left = evaluate(expr->left);

    if (expr->op->getType() == TokenType::OR)
    {
        if (isTruthy(left))
        {
            return left;
        }
    }
    else
    {
        if (!isTruthy(left))
        {
            return left;
        }
    }

    return evaluate(expr->right);
}

Object Interpreter::visitVariableExpr(const expr::Variable* expr)
{
    return this->environment->get(expr->name);
}

Object Interpreter::visitAssignExpr(const expr::Assign* expr)
{
    Object value = evaluate(expr->value);
    environment->assign(expr->name, value);

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

Object Interpreter::visitCallExpr(const expr::Call* expr)
{
    Object callee = evaluate(expr->callee);

    std::vector<Object>* arguments = new std::vector<Object>;

    for (auto argument : *expr->arguments)
    {
        arguments->push_back(evaluate(argument));
    }

    if (!ObjectParser::isCallable(callee))
    {
        throw RuntimeError(expr->paren, "Can only call functions and classes.");
    }

    LoxCallable* function = ObjectGetCallable(callee);

    if (arguments->size() != function->arity())
    {
        throw RuntimeError(expr->paren,
          "Expected " + std::to_string(function->arity()) + " arguments but got " +
            std::to_string(arguments->size()) + ".");
    }

    return function->call(this, arguments);
}

Object Interpreter::evaluate(const expr::Expr* expr)
{
    return expr->accept(this);
}

bool Interpreter::isTruthy(Object right)
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

void Interpreter::execute(const stmt::Stmt* statement)
{
    statement->accept(this);
}
