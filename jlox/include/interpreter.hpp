#pragma once

#include <expr.hpp>
#include <object.hpp>

class Interpreter : public expr::Visitor
{
  public:
    Object visitBinaryExpr(const expr::Binary* expr) override;
    Object visitGroupingExpr(const expr::Grouping* expr) override;
    Object visitLiteralExpr(const expr::Literal* expr) override;
    Object visitUnaryExpr(const expr::Unary* expr) override;

    void interpret(expr::Expr* expression);

  private:
    Object evaluate(const expr::Expr* expr);
    bool isTruthy(Object& right);
    bool isEqual(Object& left, Object& right);
    void checkNumberOperand(const Token* op, Object& operand);
    void checkNumberOperands(const Token* op, Object& left, Object& right);
    std::string stringify(Object object);
};
