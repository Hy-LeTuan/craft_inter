#include "literal_value.hpp"
#include <token.hpp>

#if TOKEN_DEBUG
#include <iostream>

Token::~Token()
{
    std::cout << "freed: " << toString() << std::endl;
}
#endif

TokenType Token::getType() const
{
    return type;
}

int Token::getLine() const
{
    return line;
}

Token::Token(TokenType type, std::string lexeme, LiteralValue literal, int line)
  : type{ type }
  , lexeme{ lexeme }
  , literal{ std::move(literal) }
  , line{ line }
{
}

std::string Token::getLexeme() const
{
    return this->lexeme;
}

LiteralValue* Token::getLiteral() const
{
    LiteralValue* l = new LiteralValue{};
    *l = literal;

    return l;
}

std::string Token::toString() const
{
    std::string literal_repr = Token::displayLiteral(literal);
    std::string name = Token::displayType(type) + " lexeme: [" + lexeme + "]";

    if (literal_repr != "Nil")
    {
        name += " literal: {" + literal_repr + "}";
    }

    return name;
}

std::string Token::displayLiteral(LiteralValue literal)
{
    return std::visit(LiteralValueVisitor{}, literal);
}

std::string Token::displayType(TokenType type)
{
    switch (type)
    {
        case TokenType::LEFT_PAREN:
            return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN:
            return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE:
            return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE:
            return "RIGHT_BRACE";
        case TokenType::COMMA:
            return "COMMA";
        case TokenType::DOT:
            return "DOT";
        case TokenType::MINUS:
            return "MINUS";
        case TokenType::PLUS:
            return "PLUS";
        case TokenType::SEMICOLON:
            return "SEMICOLON";
        case TokenType::SLASH:
            return "SLASH";
        case TokenType::STAR:
            return "STAR";
        case TokenType::BANG:
            return "BANG";
        case TokenType::BANG_EQUAL:
            return "BANG_EQUAL";
        case TokenType::EQUAL:
            return "EQUAL";
        case TokenType::EQUAL_EQUAL:
            return "EQUAL_EQUAL";
        case TokenType::GREATER:
            return "GREATER";
        case TokenType::GREATER_EQUAL:
            return "GREATER_EQUAL";
        case TokenType::LESS:
            return "LESS";
        case TokenType::LESS_EQUAL:
            return "LESS_EQUAL";
        case TokenType::IDENTIFIER:
            return "IDENTIFIER";
        case TokenType::STRING:
            return "STRING";
        case TokenType::NUMBER:
            return "NUMBER";
        case TokenType::AND:
            return "AND";
        case TokenType::CLASS:
            return "CLASS";
        case TokenType::ELSE:
            return "ELSE";
        case TokenType::FALSE:
            return "FALSE";
        case TokenType::FUN:
            return "FUN";
        case TokenType::FOR:
            return "FOR";
        case TokenType::IF:
            return "IF";
        case TokenType::NIL:
            return "NIL";
        case TokenType::OR:
            return "OR";
        case TokenType::PRINT:
            return "PRINT";
        case TokenType::RETURN:
            return "RETURN";
        case TokenType::SUPER:
            return "SUPER";
        case TokenType::THIS:
            return "THIS";
        case TokenType::TRUE:
            return "TRUE";
        case TokenType::VAR:
            return "VAR";
        case TokenType::WHILE:
            return "WHILE";
        case TokenType::__EOF__:
            return "__EOF__";
        default:
            return "";
    }
}
