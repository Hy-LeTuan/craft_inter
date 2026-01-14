#pragma once

#include "object_utils.hpp"
#include <alias.hpp>

#include <string>
#include <any>
#include <iostream>

class AstPrinter
  : public expr::Visitor
  , public stmt::Visitor
{
  public:
    Object visitBinaryExpr(const expr::Binary* expr) override;
    Object visitCallExpr(const expr::Call* expr) override;
    Object visitGetExpr(const expr::Get* expr) override;
    Object visitAssignExpr(const expr::Assign* expr) override;
    Object visitGroupingExpr(const expr::Grouping* expr) override;
    Object visitLiteralExpr(const expr::Literal* expr) override;
    Object visitLogicalExpr(const expr::Logical* expr) override;
    Object visitSetExpr(const expr::Set* expr) override;
    Object visitThisExpr(const expr::This* expr) override;
    Object visitUnaryExpr(const expr::Unary* expr) override;
    Object visitVariableExpr(const expr::Variable* expr) override;

    Object visitExpressionStmt(const stmt::Expression* stmt) override;
    Object visitIfStmt(const stmt::If* stmt) override;
    Object visitFunctionStmt(const stmt::Function* stmt) override;
    Object visitBlockStmt(const stmt::Block* stmt) override;
    Object visitClassStmt(const stmt::Class* stmt) override;
    Object visitPrintStmt(const stmt::Print* stmt) override;
    Object visitReturnStmt(const stmt::Return* stmt) override;
    Object visitVarStmt(const stmt::Var* stmt) override;
    Object visitWhileStmt(const stmt::While* stmt) override;

    void print(const stmt::Stmt* stmt);
    void printAll(const VecStmt& statements);
    std::string stringify(const stmt::Stmt* stmt);

  private:
    template<typename... Args>
    std::string parenthesize(std::string name, const Args*... exprs)
    {
        std::string output = "(";
        output.append(name);

        ((
           [&]
           {
               output.append(" ");
               try
               {
                   std::any res = exprs->accept(this);
                   output.append(ObjectParser::GetString(res));
               }
               catch (const std::bad_any_cast& e)
               {
                   std::cout << "Failed on expression: " << name << std::endl;
                   std::cout << e.what() << std::endl;
               }
           }()),
          ...);

        output.append(")");

        return output;
    }

    std::string stringify_block(const std::vector<stmt::Stmt*>* statements);
    std::string stringify_tokens(const std::vector<Token*>* params);
};
