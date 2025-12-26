#pragma once

#include <stmt.hpp>
#include <exception>
#include <token.hpp>
#include <expr.hpp>

#include <vector>

using VecStmt = std::vector<stmt::Stmt*>;

class ParseError : public std::exception
{
};

class Parser
{
  public:
    Parser(std::vector<Token*> tokens);
    ~Parser();

    std::vector<stmt::Stmt*> parse();

  private:
    int current = 0;
    std::vector<Token*> tokens;

    stmt::Stmt* declaration();
    stmt::Stmt* varDeclaration();
    stmt::Stmt* statement();
    stmt::Stmt* whileStatement();
    stmt::Stmt* forStatement();
    stmt::Stmt* ifStatement();
    VecStmt* block();
    stmt::Stmt* printStatement();
    stmt::Stmt* expressionStatement();

    expr::Expr* expression();
    expr::Expr* assignment();
    expr::Expr* logic_or();
    expr::Expr* logic_and();
    expr::Expr* equality();
    expr::Expr* comparison();
    expr::Expr* term();
    expr::Expr* factor();
    expr::Expr* unary();
    expr::Expr* primary();

    template<typename... T>
    bool match(T... types)
    {
        for (auto type : { types... })
        {
            if (check(type))
            {
                advance();
                return true;
            }
        }

        return false;
    }

    bool check(TokenType type);
    bool isAtEnd();
    void freeUnownedToken();
    void freeUnownedToken(Token* token);
    void freeExpression(expr::Expr* expr);
    Token* peek();
    Token* previous();
    Token* advance();
    Token* consume(TokenType type, std::string message);

    ParseError error(Token* token, std::string message);
    void synchronize();
};
