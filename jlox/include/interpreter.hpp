#pragma once

#include <expr.hpp>
#include <any>


class Interpreter : public Visitor
{
  public:
    std::any visitBinaryExpr(const Binary* expr) override;
    std::any visitGroupingExpr(const Grouping* expr) override;
    std::any visitLiteralExpr(const Literal* expr) override;
    std::any visitUnaryExpr(const Unary* expr) override;

    void interpret(Expr* expression);

  private:
    std::any evaluate(const Expr* expr);
    bool isTruthy(std::any& right);
    bool isEqual(std::any& left, std::any& right);
    void checkNumberOperand(const Token* op, std::any& operand);
    void checkNumberOperands(const Token* op, std::any& left, std::any& right);
    std::string stringify(std::any object);
};
