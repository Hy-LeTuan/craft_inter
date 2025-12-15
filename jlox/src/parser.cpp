#include <expr.hpp>
#include <lox.hpp>
#include <parser.hpp>
#include <token.hpp>
#include <token_type.hpp>

#include <iostream>

Expr* Parser::parse()
{
    try
    {
        return expression();
    }
    catch (ParseError)
    {
        return nullptr;
    }
}

Expr* Parser::expression()
{
    Expr* expr = equality();

    while (match(TokenType::COMMA))
    {
        expr = equality();
    }

    return expr;
}

Expr* Parser::equality()
{
    Expr* expr = comparison();

    while (match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL))
    {
        Token* op = previous();
        Expr* right = comparison();
        expr = new Binary(expr, op, right);
    }

    return expr;
}

Expr* Parser::comparison()
{
    Expr* expr = term();

    while (
      match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL))
    {
        Token* op = previous();
        Expr* right = term();

        expr = new Binary(expr, op, right);
    }

    return expr;
}

Expr* Parser::term()
{
    Expr* expr = factor();

    while (match(TokenType::PLUS, TokenType::MINUS))
    {
        Token* op = previous();
        Expr* right = term();

        expr = new Binary(expr, op, right);
    }

    return expr;
}

Expr* Parser::factor()
{
    Expr* expr = unary();

    while (match(TokenType::SLASH, TokenType::STAR))
    {
        Token* op = previous();
        Expr* right = unary();

        expr = new Binary(expr, op, right);
    }

    return expr;
}

Expr* Parser::unary()
{
    if (match(TokenType::BANG, TokenType::MINUS))
    {
        Token* op = previous();
        Expr* right = unary();

        return new Unary(op, right);
    }

    return primary();
}

Expr* Parser::primary()
{
    if (match(TokenType::FALSE))
    {
        return new Literal(new Object{ false });
    }
    else if (match(TokenType::TRUE))
    {
        return new Literal(new Object{ true });
    }
    else if (match(TokenType::NIL))
    {
        return new Literal(new Object{});
    }
    else if (match(TokenType::NUMBER, TokenType::STRING))
    {
        return new Literal(previous()->getLiteral());
    }
    else if (match(TokenType::LEFT_PAREN))
    {
        Expr* expr = expression();

        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");

        return new Grouping(expr);
    }
    else
    {
        throw error(peek(), "Expect expression.");
    }

    std::cout << "expression called with current: " << peek() << std::endl;
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
        return advance();
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
