#pragma once

#include "token_type.hpp"
#include <string>
#include <token.hpp>
#include <vector>
#include <unordered_map>

class Scanner
{
  public:
    Scanner(std::string source);
    std::vector<Token*> scanTokens();

  private:
    const std::string source;
    const std::unordered_map<std::string, TokenType> keywords;
    std::vector<Token*> tokens;

    int start;
    int current;
    int line;

    bool isAtEnd();
    void scanToken();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, LiteralValue literal);
    bool match(char expected);
    char peek();
    char peekNext();
    void string();
    void number();
    void identifier();
};
