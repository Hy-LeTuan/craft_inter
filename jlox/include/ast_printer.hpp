#pragma once

#include <stmt.hpp>
#include <expr.hpp>
#include <object.hpp>

#include <string>
#include <any>
#include <iostream>

class AstPrinter
  : public expr::Visitor
  , public stmt::Visitor
{
  public:
    Object visitBinaryExpr(const expr::Binary* expr) override;
    Object visitAssignExpr(const expr::Assign* expr) override;
    Object visitGroupingExpr(const expr::Grouping* expr) override;
    Object visitLiteralExpr(const expr::Literal* expr) override;
    Object visitLogicalExpr(const expr::Logical* expr) override;
    Object visitUnaryExpr(const expr::Unary* expr) override;
    Object visitVariableExpr(const expr::Variable* expr) override;

    Object visitExpressionStmt(const stmt::Expression* stmt) override;
    Object visitIfStmt(const stmt::If* stmt) override;
    Object visitBlockStmt(const stmt::Block* stmt) override;
    Object visitPrintStmt(const stmt::Print* stmt) override;
    Object visitVarStmt(const stmt::Var* stmt) override;
    Object visitWhileStmt(const stmt::While* stmt) override;

    void print(const stmt::Stmt* stmt);
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
                   output.append(std::any_cast<std::string>(res));
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
};
