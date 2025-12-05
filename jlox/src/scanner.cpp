#include "token_type.hpp"
#include <cctype>
#include <iostream>
#include <scanner.hpp>
#include <lox.hpp>
#include <string>

Scanner::Scanner(std::string source)
  : source{ source }
  , keywords{
      { "and", TokenType::AND },
      { "class", TokenType::CLASS },
      { "else", TokenType::ELSE },
      { "false", TokenType::FALSE },
      { "for", TokenType::FOR },
      { "fun", TokenType::FUN },
      { "if", TokenType::IF },
      { "nil", TokenType::NIL },
      { "or", TokenType::OR },
      { "print", TokenType::PRINT },
      { "return", TokenType::RETURN },
      { "super", TokenType::SUPER },
      { "this", TokenType::THIS },
      { "true", TokenType::TRUE },
      { "var", TokenType::VAR },
      { "while", TokenType::WHILE },
  }
{
    start = 0;
    current = 0;
    line = 1;
}

std::vector<Token> Scanner::scanTokens()
{
    std::cout << "source: " << source << std::endl; 
    while (!isAtEnd())
    {
        start = current;
        scanToken();
    }

    tokens.emplace_back(TokenType::__EOF__, std::string{}, Literal{}, line);

    return tokens;
}

bool Scanner::isAtEnd()
{
    return current >= source.length();
}

void Scanner::scanToken()
{
    char c = advance();

    switch (c)
    {
        case '(':
            addToken(TokenType::LEFT_PAREN);
            break;
        case ')':
            addToken(TokenType::RIGHT_PAREN);
            break;
        case '{':
            addToken(TokenType::LEFT_BRACE);
            break;
        case '}':
            addToken(TokenType::RIGHT_BRACE);
            break;
        case ',':
            addToken(TokenType::COMMA);
            break;
        case '.':
            addToken(TokenType::DOT);
            break;
        case '-':
            addToken(TokenType::MINUS);
            break;
        case '+':
            addToken(TokenType::PLUS);
            break;
        case ';':
            addToken(TokenType::SEMICOLON);
            break;
        case '*':
            addToken(TokenType::STAR);
            break;
        case '!':
            addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
            break;
        case '=':
            addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
            break;
        case '<':
            addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
            break;
        case '>':
            addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
            break;
        case '/':
            if (match('/'))
            {
                while (peek() != '\n' && !isAtEnd())
                {
                    advance();
                }
            }
            else
            {
                addToken(TokenType::SLASH);
            }
            break;
        case ' ':
        case '\r':
        case '\t':
            break;
        case '\n':
            line++;
            break;
        case '"':
            string();
            break;
        default:
            if (std::isdigit(c))
            {
                number();
            }
            else if (std::isalpha(c))
            {
                identifier();
            }
            else
            {
                Lox::error(line, "Unexpected character.");
            }
            break;
    }
}

void Scanner::identifier()
{
    while (std::isalpha(peek()))
    {
        advance();
    }

    std::string text = source.substr(start, current - start);
    auto text_it = keywords.find(text);
    TokenType type;

    if (text_it == keywords.end())
    {
        type = TokenType::IDENTIFIER;
    }
    else
    {
        type = text_it->second;
    }

    addToken(type);
}

void Scanner::number()
{
    while (std::isdigit(peek()))
    {
        advance();
    }

    if (peek() == '.' && std::isdigit(peekNext()))
    {
        advance();

        while (std::isdigit(peek()))
        {
            advance();
        }
    }

    Literal value = std::stod(source.substr(start, current - start));
    addToken(TokenType::NUMBER, value);
}

void Scanner::string()
{
    while (peek() != '"' && !isAtEnd())
    {
        if (peek() != '\n')
        {
            line++;
        }

        advance();
    }

    if (isAtEnd())
    {
        Lox::error(line, "Unterminated string.");
        return;
    }

    advance();

    Literal value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}

char Scanner::peekNext()
{
    // same at isAtEnd() but 1 forward
    if (current + 1 >= source.length())
    {
        return '\0';
    }

    return source.at(current + 1);
}

char Scanner::peek()
{
    if (isAtEnd())
    {
        return '\0';
    }
    return source.at(current);
}

bool Scanner::match(char expected)
{
    if (isAtEnd())
    {
        return false;
    }

    if (source.at(current) != expected)
    {
        return false;
    }

    current++;
    return true;
}

char Scanner::advance()
{
    return source.at(current++);
}

void Scanner::addToken(TokenType type)
{
    addToken(type, Literal{});
}

void Scanner::addToken(TokenType type, Literal literal)
{
    std::string text = source.substr(start, current - start);
    
    std::cout << "start: " << start << std::endl;
    std::cout << "current: " << current << std::endl;
    std::cout << "text received: " << text << std::endl;

    tokens.emplace_back(type, text, literal, line);
}
