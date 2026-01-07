#include <parser.hpp>

#include <stmt.hpp>
#include <expr.hpp>
#include <lox.hpp>
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

VecStmt Parser::parse()
{
    VecStmt statements;

    while (!isAtEnd())
    {
        statements.push_back(declaration());
    }

    return statements;
}

stmt::Stmt* Parser::declaration()
{
    try
    {
        if (match(TokenType::FUN))
        {
            freeUnownedToken();
            return function("function");
        }
        else if (match(TokenType::VAR))
        {
            freeUnownedToken();
            return varDeclaration();
        }
        else
        {
            return statement();
        }
    }
    catch (ParseError)
    {
        synchronize();
        return nullptr;
    }
}

stmt::Stmt* Parser::function(std::string kind)
{
    Token* name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<Token*>* parameters = new std::vector<Token*>;

    if (!check(TokenType::RIGHT_PAREN))
    {
        do
        {
            if (previous()->getType() == TokenType::COMMA)
            {
                freeUnownedToken();
            }

            if (parameters->size() >= 255)
            {
                error(peek(), "Can't have more than 255 parameters");
            }

            parameters->push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
        } while (match(TokenType::COMMA));
    }

    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

    consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
    VecStmt* body = block();

    return new stmt::Function(name, parameters, body);
}

stmt::Stmt* Parser::varDeclaration()
{
    Token* name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    Expr* initializer = nullptr;

    if (match(TokenType::EQUAL))
    {
        freeUnownedToken();
        initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    freeUnownedToken();

    return new stmt::Var(name, initializer);
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
    else if (match(TokenType::RETURN))
    {
        return returnStatement();
    }
    else if (match(TokenType::LEFT_BRACE))
    {
        freeUnownedToken();
        auto statements = block();
        return new stmt::Block(statements);
    }
    else if (match(TokenType::IF))
    {
        freeUnownedToken();
        return ifStatement();
    }
    else if (match(TokenType::WHILE))
    {
        freeUnownedToken();
        return whileStatement();
    }
    else if (match(TokenType::FOR))
    {
        freeUnownedToken();
        return forStatement();
    }
    else
    {
        return expressionStatement();
    }
}

stmt::Stmt* Parser::returnStatement()
{
    Token* keyword = previous();
    Expr* value = nullptr;

    if (!check(TokenType::SEMICOLON))
    {
        value = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ; after return value.");
    freeUnownedToken();

    return new stmt::Return(keyword, value);
}

stmt::Stmt* Parser::forStatement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");
    freeUnownedToken();

    stmt::Stmt* initializer = nullptr;

    if (match(TokenType::SEMICOLON))
    {
        freeUnownedToken();
        initializer = nullptr;
    }
    else if (match(TokenType::VAR))
    {
        freeUnownedToken();
        initializer = varDeclaration();
    }
    else
    {
        initializer = expressionStatement();
    }

    expr::Expr* condition = nullptr;

    if (!check(TokenType::SEMICOLON))
    {
        condition = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");
    freeUnownedToken();

    Expr* increment = nullptr;

    if (!check(TokenType::RIGHT_PAREN))
    {
        increment = expression();
    }

    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");
    freeUnownedToken();

    stmt::Stmt* body = statement();

    if (increment)
    {
        body = new stmt::Block(new VecStmt{ body, new stmt::Expression{ increment } });
    }

    if (!condition)
    {
        condition = new expr::Literal{ new LiteralValue{ true } };
    }

    body = new stmt::While{ condition, body };

    if (initializer)
    {
        body = new stmt::Block{ new VecStmt{ initializer, body } };
    }

    return body;
}

stmt::Stmt* Parser::whileStatement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    freeUnownedToken();

    Expr* condition = expression();

    consume(TokenType::RIGHT_PAREN, "Expect ')' after 'while'.");
    freeUnownedToken();

    stmt::Stmt* body = statement();

    return new stmt::While{ condition, body };
}

stmt::Stmt* Parser::ifStatement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after if.");
    freeUnownedToken();

    Expr* condition = expression();

    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");
    freeUnownedToken();

    stmt::Stmt* thenBranch = statement();
    stmt::Stmt* elseBranch = nullptr;

    if (match(TokenType::ELSE))
    {
        freeUnownedToken();
        elseBranch = statement();
    }

    return new stmt::If{ condition, thenBranch, elseBranch };
}

VecStmt* Parser::block()
{
    VecStmt* statements = new VecStmt{};

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
    {
        statements->push_back(declaration());
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after block");
    freeUnownedToken();

    return statements;
}

stmt::Stmt* Parser::printStatement()
{
    Expr* value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    freeUnownedToken();

    return new stmt::Print{ value };
}

stmt::Stmt* Parser::expressionStatement()
{
    Expr* value = expression();

    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    freeUnownedToken();

    return new stmt::Expression{ value };
}

expr::Expr* Parser::expression()
{
    expr::Expr* expr = assignment();

    // while (match(TokenType::COMMA))
    // {
    //     freeUnownedToken();
    //     expr = assignment();
    // }

    return expr;
}

expr::Expr* Parser::assignment()
{
    expr::Expr* expr = logic_or();

    if (match(TokenType::EQUAL))
    {
        Token* equals = previous();
        expr::Expr* value = assignment();

        expr::Variable* expr_var = dynamic_cast<expr::Variable*>(expr);

        if (expr_var)
        {
            Token* name = expr_var->name->clone();

            freeUnownedToken(equals);
            freeExpression(expr_var);

            return new expr::Assign{ name, value };
        }
        else
        {
            error(std::move(equals), "Invalid assignment target");
        }
    }

    return expr;
}

expr::Expr* Parser::logic_or()
{
    Expr* expr = logic_and();

    while (match(TokenType::OR))
    {
        Token* op = previous();
        Expr* right = logic_and();
        expr = new expr::Logical(expr, op, right);
    }

    return expr;
}

expr::Expr* Parser::logic_and()
{
    Expr* expr = equality();

    while (match(TokenType::AND))
    {
        Token* op = previous();
        Expr* right = equality();
        expr = new expr::Logical(expr, op, right);
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
    else
    {
        return call();
    }
}

expr::Expr* Parser::call()
{
    expr::Expr* expr = primary();

    while (true)
    {
        if (match(TokenType::LEFT_PAREN))
        {
            freeUnownedToken();
            expr = finishCall(expr);
        }
        else
        {
            break;
        }
    }

    return expr;
}

expr::Expr* Parser::finishCall(expr::Expr* expr)
{
    VecExpr* arguments = new VecExpr;

    if (!check(TokenType::RIGHT_PAREN))
    {
        arguments->push_back(expression());

        while (match(TokenType::COMMA))
        {
            freeUnownedToken();
            if (arguments->size() >= 255)
            {
                error(peek(), "Can't have more than 255 arguments");
            }

            arguments->push_back(expression());
        }
    }

    Token* paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

    return new expr::Call(expr, paren, arguments);
}

expr::Expr* Parser::primary()
{

    if (match(TokenType::LEFT_PAREN))
    {
        freeUnownedToken();
        expr::Expr* expr = expression();

        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        freeUnownedToken();

        return new expr::Grouping(expr);
    }
    else if (match(TokenType::IDENTIFIER))
    {
        return new expr::Variable(std::move(previous()));
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
        else
        {
            return literalExpression;
        }
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

void Parser::freeUnownedToken(Token* token)
{
    delete token;
}

void Parser::freeExpression(expr::Expr* expr)
{
    delete expr;
}
