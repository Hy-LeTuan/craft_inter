#pragma once

#include <expr.hpp>

#include <string>
#include <any>
#include <iostream>

class AstPrinter : public expr::Visitor
{
  public:
    std::any visitBinaryExpr(const expr::Binary* expr) override;
    std::any visitGroupingExpr(const expr::Grouping* expr) override;
    std::any visitLiteralExpr(const expr::Literal* expr) override;
    std::any visitUnaryExpr(const expr::Unary* expr) override;

    std::string print(expr::Expr* expr);

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
