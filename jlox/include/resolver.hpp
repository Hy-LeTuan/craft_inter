#pragma once

#include <alias.hpp>
#include <interpreter.hpp>

#include <deque>
#include <unordered_map>

#define PEEK(scopes) scopes.front()
#define LEXEME(name) name->getLexeme()

enum class FunctionType
{
    NONE,
    FUNCTION
};

class Resolver
  : public stmt::Visitor
  , expr::Visitor
{
  public:
    Resolver(Interpreter* interpreter);

    Object visitExpressionStmt(const stmt::Expression* stmt) override;
    Object visitIfStmt(const stmt::If* stmt) override;
    Object visitFunctionStmt(const stmt::Function* stmt) override;
    Object visitBlockStmt(const stmt::Block* stmt) override;
    Object visitClassStmt(const stmt::Class* stmt) override;
    Object visitPrintStmt(const stmt::Print* stmt) override;
    Object visitReturnStmt(const stmt::Return* stmt) override;
    Object visitVarStmt(const stmt::Var* stmt) override;
    Object visitWhileStmt(const stmt::While* stmt) override;

    Object visitBinaryExpr(const expr::Binary* expr) override;
    Object visitCallExpr(const expr::Call* expr) override;
    Object visitGetExpr(const expr::Get* expr) override;
    Object visitAssignExpr(const expr::Assign* expr) override;
    Object visitGroupingExpr(const expr::Grouping* expr) override;
    Object visitLiteralExpr(const expr::Literal* expr) override;
    Object visitLogicalExpr(const expr::Logical* expr) override;
    Object visitUnaryExpr(const expr::Unary* expr) override;
    Object visitVariableExpr(const expr::Variable* expr) override;

    void resolve(const VecStmt* statements);

  private:
    void beginScope();

    /*
     * Resolve the declaration and definition of variables found in statements and expressions
     * */
    void resolve(const stmt::Stmt* statement);
    void resolve(const expr::Expr* expression);

    void resolveFunction(const stmt::Function* stmt, FunctionType type);
    void resolveLocal(const expr::Expr* expression, const Token* name);
    void endScope();

    void declare(const Token* name);
    void define(const Token* name);

    std::deque<std::unordered_map<std::string, bool>> scopes;
    Interpreter* interpreter;

    FunctionType currentFunction = FunctionType::NONE;
};
