#pragma once

#include <expr.hpp>

#include <string>
#include <any>
#include <iostream>

class AstPrinter : public Visitor
{
  public:
    std::any visitBinaryExpr(const Binary* expr)override;
    std::any visitGroupingExpr(const Grouping* expr) override;
    std::any visitLiteralExpr(const Literal* expr) override;
    std::any visitUnaryExpr(const Unary* expr) override;

    std::string print(Expr* expr);

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
