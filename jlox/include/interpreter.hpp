#pragma once

#include <expr.hpp>
#include <stmt.hpp>
#include <object.hpp>
#include <environment.hpp>

#include <vector>

class Interpreter
  : public expr::Visitor
  , public stmt::Visitor
{
  public:
    Object visitIfStmt(const stmt::If* stmt) override;
    Object visitBlockStmt(const stmt::Block* stmt) override;
    Object visitExpressionStmt(const stmt::Expression* stmt) override;
    Object visitPrintStmt(const stmt::Print* stmt) override;
    Object visitVarStmt(const stmt::Var* stmt) override;

    Object visitLogicalExpr(const expr::Logical* expr) override;
    Object visitBinaryExpr(const expr::Binary* expr) override;
    Object visitGroupingExpr(const expr::Grouping* expr) override;
    Object visitLiteralExpr(const expr::Literal* expr) override;
    Object visitUnaryExpr(const expr::Unary* expr) override;
    Object visitVariableExpr(const expr::Variable* expr) override;
    Object visitAssignExpr(const expr::Assign* expr) override;

    void interpret(std::vector<stmt::Stmt*> statements);

  private:
    void executeBlock(const vector<stmt::Stmt*>* statements, Environment* env);
    void execute(const stmt::Stmt* statement);
    Object evaluate(const expr::Expr* expr);
    bool isTruthy(Object& right);
    bool isEqual(Object& left, Object& right);
    void checkNumberOperand(const Token* op, Object& operand);
    void checkNumberOperands(const Token* op, Object& left, Object& right);
    std::string stringify(Object& object);

    Environment* environment = new Environment{};
};
