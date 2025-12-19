#include <expr.hpp>
#include <lox.hpp>
#include <parser.hpp>
#include <token.hpp>
#include <token_type.hpp>

Parser::Parser(std::vector<Token*> tokens)
  : tokens{ std::move(tokens) }
{
}

Parser::~Parser()
{
    if (!tokens.empty())
    {
        // free eof token
        delete tokens[tokens.size() - 1];
    }
}

std::vector<stmt::Stmt*> Parser::parse()
{
    std::vector<stmt::Stmt*> statements;

    while (!isAtEnd())
    {
        statements.push_back(statement());
    }

    return statements;
}

stmt::Stmt* Parser::statement()
{
    if (match(TokenType::PRINT))
    {
        // this also needs to be here for the same reason as in the function `primary()`
        // read it for full explaination
        freeUnownedToken();

        return printStatement();
    }

    return expressionStatement();
}

stmt::Stmt* Parser::printStatement()
{
    Expr* value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");

    return new stmt::Print{ value };
}

stmt::Stmt* Parser::expressionStatement()
{
    Expr* value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");

    return new stmt::Expression{ value };
}

expr::Expr* Parser::expression()
{
    expr::Expr* expr = equality();

    while (match(TokenType::COMMA))
    {
        expr = equality();
    }

    return expr;
}

expr::Expr* Parser::equality()
{
    expr::Expr* expr = comparison();

    while (match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL))
    {
        Token* op = previous();
        expr::Expr* right = comparison();
        expr = new expr::Binary(expr, op, right);
    }

    return expr;
}

expr::Expr* Parser::comparison()
{
    expr::Expr* expr = term();

    while (
      match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL))
    {
        Token* op = previous();
        expr::Expr* right = term();

        expr = new expr::Binary(expr, op, right);
    }

    return expr;
}

expr::Expr* Parser::term()
{
    expr::Expr* expr = factor();

    while (match(TokenType::PLUS, TokenType::MINUS))
    {
        Token* op = previous();
        expr::Expr* right = term();

        expr = new expr::Binary(expr, op, right);
    }

    return expr;
}

expr::Expr* Parser::factor()
{
    expr::Expr* expr = unary();

    while (match(TokenType::SLASH, TokenType::STAR))
    {
        Token* op = previous();
        expr::Expr* right = unary();

        expr = new expr::Binary(expr, op, right);
    }

    return expr;
}

expr::Expr* Parser::unary()
{
    if (match(TokenType::BANG, TokenType::MINUS))
    {
        Token* op = previous();
        expr::Expr* right = unary();

        return new expr::Unary(op, right);
    }

    return primary();
}

expr::Expr* Parser::primary()
{

    if (match(TokenType::LEFT_PAREN))
    {
        expr::Expr* expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");

        return new expr::Grouping(expr);
    }
    else
    {
        Expr* literalExpression = nullptr;

        if (match(TokenType::FALSE))
        {
            literalExpression = new expr::Literal(new LiteralValue{ false });
        }
        else if (match(TokenType::TRUE))
        {
            literalExpression = new expr::Literal(new LiteralValue{ true });
        }
        else if (match(TokenType::NIL))
        {
            literalExpression = new expr::Literal(new LiteralValue{});
        }
        else if (match(TokenType::NUMBER, TokenType::STRING))
        {
            literalExpression = new expr::Literal(previous()->getLiteral());
        }

        // only free tokens that contain literals.
        // this shouldn't even exist because this token should be moved
        // into the expr::Literal class to comply with RAII like any other
        // tokens, but for the sake of closely following the syntax of the book,
        // I manually free it here instead.
        freeUnownedToken();

        if (!literalExpression)
        {
            throw error(peek(), "Expect expression.");
        }

        return literalExpression;
    }
}

bool Parser::check(TokenType type)
{
    if (isAtEnd())
    {
        return false;
    }

    bool res = peek()->getType() == type;

    return res;
}

bool Parser::isAtEnd()
{
    return current >= tokens.size() - 1;
}

Token* Parser::peek()
{
    return tokens.at(current);
}

Token* Parser::previous()
{
    return tokens.at(current - 1);
}

Token* Parser::advance()
{
    if (!isAtEnd())
    {
        current++;
    }

    return previous();
}

Token* Parser::consume(TokenType type, std::string message)
{
    if (check(type))
    {
        auto e = advance();
        freeUnownedToken();

        return e;
    }

    throw error(peek(), message);
}

ParseError Parser::error(Token* token, std::string message)
{
    Lox::error(token, message);
    return ParseError();
}

void Parser::synchronize()
{
    advance();

    while (!isAtEnd())
    {
        if (previous()->getType() == TokenType::SEMICOLON)
        {
            return;
        }

        switch (peek()->getType())
        {
            case TokenType::CLASS:
            case TokenType::FUN:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
                return;
            default:
                advance();
        }
    }
}

void Parser::freeUnownedToken()
{
    delete previous();
}
