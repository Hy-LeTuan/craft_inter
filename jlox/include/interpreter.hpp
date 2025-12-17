#pragma once

#include <expr.hpp>
#include <object.hpp>

class Interpreter : public Visitor
{
  public:
    Object visitBinaryExpr(const Binary* expr) override;
    Object visitGroupingExpr(const Grouping* expr) override;
    Object visitLiteralExpr(const Literal* expr) override;
    Object visitUnaryExpr(const Unary* expr) override;

    void interpret(Expr* expression);

  private:
    Object evaluate(const Expr* expr);
    bool isTruthy(Object& right);
    bool isEqual(Object& left, Object& right);
    void checkNumberOperand(const Token* op, Object& operand);
    void checkNumberOperands(const Token* op, Object& left, Object& right);
    std::string stringify(Object object);
};
