#include "compiler.h"
#include "scanner.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
} Parser;

typedef enum
{
    // precedence are sorted from lowest to highest
    PREC_NONE,
    PREC_ASSIGNMENT, // =
    PREC_OR,         // or
    PREC_AND,        // and
    PREC_EQUALITY,   // == !=
    PREC_COMPARISON, // < > <= >=
    PREC_TERM,       // + -
    PREC_FACTOR,     // * /
    PREC_UNARY,      // ! -
    PREC_CALL,       // . ()
    PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)(bool canAssign);
typedef struct
{
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

typedef struct
{
    Token name;
    int depth;
} Local;

typedef struct
{
    Local locals[UINT8_COUNT];
    int localCount;
    int scopeDepth;
} Compiler;

Parser parser;
Compiler* current = NULL;
Chunk* compilingChunk;

static Chunk* currentChunk()
{
    return compilingChunk;
}

static void errorAt(Token* token, const char* message)
{
    if (parser.panicMode)
    {
        return;
    }

    parser.panicMode = true;
    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF)
    {
        fprintf(stderr, " at end");
    }
    else if (token->type == TOKEN_ERROR)
    {
        // Nothing.
    }
    else
    {
        fprintf(stderr, " at '%.*s'.", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser.hadError = true;
}

static void error(const char* message)
{
    errorAt(&parser.previous, message);
}

static void errorAtCurrent(const char* message)
{
    errorAt(&parser.current, message);
}

static void advance()
{
    parser.previous = parser.current;

    for (;;)
    {
        parser.current = scanToken();

        // if we get a valid token, process it
        if (parser.current.type != TOKEN_ERROR)
        {
            break;
        }

        errorAtCurrent(parser.current.start);
    }
}

static void consume(TokenType type, const char* message)
{
    if (parser.current.type == type)
    {
        advance();
        return;
    }

    errorAtCurrent(message);
}

static bool check(TokenType type)
{
    return parser.current.type == type;
}

static bool match(TokenType type)
{
    if (!check(type))
    {
        return false;
    }

    advance();
    return true;
}

static void emitByte(uint8_t byte)
{
    writeChunk(currentChunk(), byte, parser.previous.line);
}

static int emitJump(uint8_t instruction)
{
    emitByte(instruction);
    emitByte(0xff);
    emitByte(0xff);
    return currentChunk()->count - 2;
}

static void emitBytes(uint8_t byte1, uint8_t byte2)
{
    emitByte(byte1);
    emitByte(byte2);
}

static void emitLoop(int loopStart)
{
    emitByte(OP_LOOP);

    int offset = currentChunk()->count - loopStart + 2;
    if (offset > UINT16_MAX)
    {
        error("Loop body too large.");
    }

    emitByte((offset >> 8) & 0xff);
    emitByte(offset & 0xff)
}

static void emitReturn()
{
    emitByte(OP_RETURN);
}

static uint8_t makeConstant(Value value)
{
    int constant = addConstant(currentChunk(), value);

    if (constant > UINT8_MAX)
    {
        error("Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t)constant;
}

static void emitConstant(Value value)
{
    uint8_t constant = makeConstant(value);
    emitBytes(OP_CONSTANT, constant);
}

static void patchJump(int offset)
{
    int jump = currentChunk()->count - offset - 2;

    if (jump > UINT16_MAX)
    {
        error("Too much code to jump over.");
    }

    currentChunk()->code[offset] = (jump >> 8) & 0xff;
    currentChunk()->code[offset + 1] = jump & 0xff;
}

static void initCompiler(Compiler* compiler)
{
    compiler->localCount = 0;
    compiler->scopeDepth = 0;
    current = compiler;
}

static void endCompiler()
{
    emitReturn();
#ifdef DEBUG_PRINT_CODE
    if (!parser.hadError)
    {
        disassembleChunk(currentChunk(), "code");
    }
#endif
}

static void beginScope()
{
    current->scopeDepth++;
}

static void endScope()
{
    current->scopeDepth--;

    while (current->localCount > 0 &&
      current->locals[current->localCount - 1].depth > current->scopeDepth)
    {
        emitByte(OP_POP);
        current->localCount--;
    }
}

static void expression();
static void statement();
static void printStatement();
static void declaration();
static void parsePrecedence(Precedence precedence);
static uint8_t parseVariable(const char* errorMessage);
static void defineVariable(uint8_t global);
static uint8_t identifierConstant(Token* name);
static int resolveLocal(Compiler* compiler, Token* name);
static ParseRule* getRule(TokenType type);

static void binary(bool canAssign)
{
    TokenType operatorType = parser.previous.type;
    ParseRule* rule = getRule(operatorType);
    parsePrecedence((Precedence)(rule->precedence + 1));

    switch (operatorType)
    {
        case TOKEN_BANG_EQUAL:
            emitBytes(OP_EQUAL, OP_NOT);
            break;
        case TOKEN_EQUAL_EQUAL:
            emitByte(OP_EQUAL);
            break;
        case TOKEN_GREATER:
            emitByte(OP_GREATER);
            break;
        case TOKEN_GREATER_EQUAL:
            emitBytes(OP_LESS, OP_NOT);
            break;
        case TOKEN_LESS:
            emitByte(OP_LESS);
            break;
        case TOKEN_LESS_EQUAL:
            emitBytes(OP_GREATER, OP_NOT);
        case TOKEN_PLUS:
            emitByte(OP_ADD);
            break;
        case TOKEN_MINUS:
            emitByte(OP_SUBTRACT);
            break;
        case TOKEN_STAR:
            emitByte(OP_MULTIPLY);
            break;
        case TOKEN_SLASH:
            emitByte(OP_DIVIDE);
            break;
        default:
            return; // Unreachable.
    }
}

static void literal(bool canAssign)
{
    switch (parser.previous.type)
    {
        case TOKEN_FALSE:
            emitByte(OP_FALSE);
            break;
        case TOKEN_NIL:
            emitByte(OP_NIL);
            break;
        case TOKEN_TRUE:
            emitByte(OP_TRUE);
            break;

        default:
            return;
    }
}

static void number(bool canAssign)
{
    double value = strtod(parser.previous.start, NULL);
    emitConstant(NUMBER_VAL(value));
}

static void or_(bool canAssign)
{
    int elseJump = emitJump(OP_JUMP_IF_FALSE);
    int endJump = emitJump(OP_JUMP);

    patchJump(elseJump);
    emitByte(OP_POP);

    parsePrecedence(PREC_OR);

    patchJump(endJump);
}

static void and_(bool canAssign)
{
    int endJump = emitJump(OP_JUMP_IF_FALSE);

    emitByte(OP_POP);
    parsePrecedence(PREC_AND);

    patchJump(endJump);
}

static void string(bool canAssign)
{
    emitConstant(OBJ_VAL(copyString(parser.previous.start + 1, parser.previous.length - 2)));
}

static void namedVariable(Token name, bool canAssign)
{
    uint8_t getOp, setOp;
    int arg = resolveLocal(current, &name);

    if (arg != -1)
    {
        getOp = OP_GET_LOCAL;
        setOp = OP_SET_LOCAL;
    }
    else
    {
        arg = identifierConstant(&name);
        getOp = OP_GET_GLOBAL;
        setOp = OP_SET_GLOBAL;
    }

    if (canAssign && match(TOKEN_EQUAL))
    {
        expression();
        emitBytes(setOp, (uint8_t)arg);
    }
    else
    {
        emitBytes(getOp, arg);
    }
}

static void variable(bool canAssign)
{
    namedVariable(parser.previous, canAssign);
}

static void unary(bool canAssign)
{
    TokenType operatorType = parser.previous.type;

    parsePrecedence(PREC_UNARY);

    switch (operatorType)
    {
        case TOKEN_BANG:
            emitByte(OP_NOT);
            break;
        case TOKEN_MINUS:
            emitByte(OP_NEGATE);
            break;
        default:
            return;
    }
}

static void grouping(bool canAssign)
{
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void expression()
{
    parsePrecedence(PREC_ASSIGNMENT);
}

static void block()
{
    while (!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF))
    {
        declaration();
    }

    consume(TOKEN_RIGHT_BRACE, "Expect '}' after block.");
}

static void varDeclaration()
{
    uint8_t global = parseVariable("Expect variable name.");

    if (match(TOKEN_EQUAL))
    {
        expression();
    }
    else
    {
        emitByte(OP_NIL);
    }

    consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration");

    defineVariable(global);
}

static void expressionStatement()
{
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
    emitByte(OP_POP);
}

static void forStatement()
{
    beginScope();
    consume(TOKEN_LEFT_PAREN, "Expect '(' after for.");

    if (match(TOKEN_SEMICOLON))
    {
        // no initializer
    }
    else if (match(TOKEN_VAR))
    {
        varDeclaration();
    }
    else
    {
        expressionStatement();
    }

    int loopStart = currentChunk()->count;
    int exitJump = -1;

    if (!match(TOKEN_SEMICOLON))
    {
        // we want the value to be on the stack, so no expressionStatement here
        expression();
        consume(TOKEN_SEMICOLON, "Expect ';' after loop condition.");

        exitJump = emitJump(OP_JUMP_IF_FALSE);
        emitByte(OP_POP);
    }

    if (!match(TOKEN_RIGHT_PAREN))
    {
        int bodyJump = emitJump(OP_JUMP);
        int incrementStart = currentChunk()->count;
        expression();
        emitByte(OP_POP);
        consume(TOKEN_RIGHT_PAREN, "Expect ')' after for clause.");

        emitLoop(loopStart);
        loopStart = incrementStart;
        patchJump(bodyJump);
    }

    statement();
    emitLoop(loopStart);

    if (exitJump != -1)
    {
        patchJump(exitJump);
        emitByte(OP_POP);
    }

    endScope();
}

static void ifStatement()
{
    consume(TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

    int thenJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);
    statement();

    int elseJump = emitJump(OP_JUMP);

    // has to happen after the else jump bytecode is compiled, since if the then branch is executed,
    // then it is going to end up exactly at where the elseJump is. if not, we execute the else
    // statement, skipping over the elseJump bytecode
    patchJump(thenJump);
    emitByte(OP_POP);

    if (match(TOKEN_ELSE))
    {
        statement();
    }
    patchJump(elseJump);
}

static void printStatement()
{
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after value.");
    emitByte(OP_PRINT);
}

static void whileStatement()
{
    int loopStart = currentChunk()->count;

    consume(TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

    int exitJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);
    statement();

    emitLoop(loopStart);

    patchJump(exitJump);
    emitByte(OP_POP);
}

static void synchronize()
{
    parser.panicMode = false;

    while (parser.current.type != TOKEN_EOF)
    {
        if (parser.current.type == TOKEN_SEMICOLON)
        {
            return;
        }
        switch (parser.current.type)
        {
            case TOKEN_CLASS:
            case TOKEN_FUN:
            case TOKEN_VAR:
            case TOKEN_FOR:
            case TOKEN_IF:
            case TOKEN_WHILE:
            case TOKEN_PRINT:
            case TOKEN_RETURN:
                return;

            default:;
        }
    }

    advance();
}

static void declaration()
{
    if (match(TOKEN_VAR))
    {
        varDeclaration();
    }
    else
    {
        statement();
    }

    if (parser.panicMode)
    {
        synchronize();
    }
}

static void statement()
{
    if (match(TOKEN_PRINT))
    {
        printStatement();
    }
    else if (match(TOKEN_FOR))
    {
        forStatement();
    }
    else if (match(TOKEN_IF))
    {
        ifStatement();
    }
    else if (match(TOKEN_WHILE))
    {
        whileStatement();
    }
    else if (match(TOKEN_LEFT_BRACE))
    {
        beginScope();
        block();
        endScope();
    }
    else
    {
        expressionStatement();
    }
}

ParseRule rules[] = {
    [TOKEN_LEFT_PAREN] = { grouping, NULL, PREC_NONE },
    [TOKEN_RIGHT_PAREN] = { NULL, NULL, PREC_NONE },
    [TOKEN_LEFT_BRACE] = { NULL, NULL, PREC_NONE },
    [TOKEN_RIGHT_BRACE] = { NULL, NULL, PREC_NONE },
    [TOKEN_COMMA] = { NULL, NULL, PREC_NONE },
    [TOKEN_DOT] = { NULL, NULL, PREC_NONE },
    [TOKEN_MINUS] = { unary, binary, PREC_TERM },
    [TOKEN_PLUS] = { NULL, binary, PREC_TERM },
    [TOKEN_SEMICOLON] = { NULL, NULL, PREC_NONE },
    [TOKEN_SLASH] = { NULL, binary, PREC_FACTOR },
    [TOKEN_STAR] = { NULL, binary, PREC_FACTOR },
    [TOKEN_BANG] = { unary, NULL, PREC_NONE },
    [TOKEN_BANG_EQUAL] = { NULL, binary, PREC_EQUALITY },
    [TOKEN_EQUAL] = { NULL, NULL, PREC_NONE },
    [TOKEN_EQUAL_EQUAL] = { NULL, binary, PREC_EQUALITY },
    [TOKEN_GREATER] = { NULL, binary, PREC_COMPARISON },
    [TOKEN_GREATER_EQUAL] = { NULL, binary, PREC_COMPARISON },
    [TOKEN_LESS] = { NULL, binary, PREC_COMPARISON },
    [TOKEN_LESS_EQUAL] = { NULL, binary, PREC_COMPARISON },
    [TOKEN_IDENTIFIER] = { variable, NULL, PREC_NONE },
    [TOKEN_STRING] = { string, NULL, PREC_NONE },
    [TOKEN_NUMBER] = { number, NULL, PREC_NONE },
    [TOKEN_AND] = { NULL, and_, PREC_AND },
    [TOKEN_CLASS] = { NULL, NULL, PREC_NONE },
    [TOKEN_ELSE] = { NULL, NULL, PREC_NONE },
    [TOKEN_FALSE] = { literal, NULL, PREC_NONE },
    [TOKEN_FOR] = { NULL, NULL, PREC_NONE },
    [TOKEN_FUN] = { NULL, NULL, PREC_NONE },
    [TOKEN_IF] = { NULL, NULL, PREC_NONE },
    [TOKEN_NIL] = { literal, NULL, PREC_NONE },
    [TOKEN_OR] = { NULL, or_, PREC_NONE },
    [TOKEN_PRINT] = { NULL, NULL, PREC_NONE },
    [TOKEN_RETURN] = { NULL, NULL, PREC_NONE },
    [TOKEN_SUPER] = { NULL, NULL, PREC_NONE },
    [TOKEN_THIS] = { NULL, NULL, PREC_NONE },
    [TOKEN_TRUE] = { literal, NULL, PREC_NONE },
    [TOKEN_VAR] = { NULL, NULL, PREC_NONE },
    [TOKEN_WHILE] = { NULL, NULL, PREC_NONE },
    [TOKEN_ERROR] = { NULL, NULL, PREC_NONE },
    [TOKEN_EOF] = { NULL, NULL, PREC_NONE },
};

/*
 * This function lies at the heart of the Vaughan Pratt parsing method. The idea is built on the
 * fact that every expression, by definition of this parsing method, starts with a prefix token.
 * This token maybe a number, an open bracket, an identifier, or anything that is allowed by the
 * language and scannable by the scanner (lexer). Then, since this token is definitely a part of
 * some prefix operation, we retrieve the correct function to compile this prefix operation from the
 * call table. What happens afterwards is that since we only have 1 lookahead token, we only know
 * after compiling the prefix operation whether it is a standalone expression or an operand of an
 * infix operator. If it is, then we continually compile the remaining part of the infix operation,
 * only stopping until there is no more infix operation or if the infix operation has a lower
 * precedence than the STARTING prefix operation.
 */
static void parsePrecedence(Precedence precedence)
{
    advance();
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;

    if (prefixRule == NULL)
    {
        error("Expect expression.");
        return;
    }

    bool canAssign = precedence <= PREC_ASSIGNMENT;
    prefixRule(canAssign);

    // the prefix expression we just compiled might be an operand to an infix
    // operator, so we check to see if that's the case.
    // we continue to check whether what we just compiled is an operand to an infix operator, and if
    // it is, we continue compiling it.
    while (precedence <= getRule(parser.current.type)->precedence)
    {
        advance();
        ParseFn infixRule = getRule(parser.previous.type)->infix;

        if (infixRule == NULL)
        {
            error("Expect infix operand.");
            return;
        }

        infixRule(canAssign);
    }

    if (canAssign && match(TOKEN_EQUAL))
    {
        error("Invalid assignment target.");
    }
}

static uint8_t identifierConstant(Token* name)
{
    return makeConstant(OBJ_VAL(copyString(name->start, name->length)));
}

static bool identifiersEqual(Token* a, Token* b)
{
    if (a->length != b->length)
    {
        return false;
    }

    return memcmp(a->start, b->start, a->length) == 0;
}

static int resolveLocal(Compiler* compiler, Token* name)
{
    for (int i = compiler->localCount - 1; i >= 0; i--)
    {
        Local* local = &compiler->locals[i];

        /*
         * the index for a local on the compiler is the same as its index on the stack because every
         * expression moves the stack 1, and since local variables, if initialized will have an
         * accompany expression that moves the stack by 1, and other declaration does not modify the
         * stack, when a local variable is added, it's value is also at the top of the stack by the
         * time of recording. Then, if more variables are added, the locals and stack keeps
         * incrementing at the same pace
         * */
        if (identifiersEqual(name, &local->name))
        {
            if (local->depth == -1)
            {
                error("Can't read local variable in its own initializer.");
            }
            return i;
        }
    }

    return -1;
}

static void addLocal(Token name)
{
    if (current->localCount == UINT8_COUNT)
    {
        error("Too many local variables in function.");
        return;
    }

    Local* local = &current->locals[current->localCount++];
    local->name = name;
    local->depth = -1;
}

static void declareVariable()
{
    if (current->scopeDepth == 0)
    {
        return;
    }

    Token* name = &parser.previous;

    for (int i = current->localCount - 1; i >= 0; i--)
    {
        Local* local = &current->locals[i];

        // not in the current scope
        if (local->depth != -1 && local->depth < current->scopeDepth)
        {
            break;
        }

        if (identifiersEqual(name, &local->name))
        {
            error("Already a variable with this name in this scope.");
        }
    }

    addLocal(*name);
}

static uint8_t parseVariable(const char* errorMessage)
{
    consume(TOKEN_IDENTIFIER, errorMessage);

    declareVariable();

    if (current->scopeDepth > 0)
    {
        return 0;
    }

    return identifierConstant(&parser.previous);
}

static void markInitialized()
{
    current->locals[current->localCount - 1].depth = current->scopeDepth;
}

static void defineVariable(uint8_t global)
{
    if (current->scopeDepth > 0)
    {
        markInitialized();
        return;
    }

    emitBytes(OP_DEFINE_GLOBAL, global);
}

static ParseRule* getRule(TokenType type)
{
    return &rules[type];
}

bool compile(const char* source, Chunk* chunk)
{
    initScanner(source);
    Compiler compiler;
    initCompiler(&compiler);

    compilingChunk = chunk;

    parser.hadError = false;
    parser.panicMode = false;

    advance();

#ifdef DEBUG_PRINT_CODE
    printf("compile is called, reach statement parsing.\n");
#endif

    while (!match(TOKEN_EOF))
    {
        declaration();
    }

    // expression();
    // consume(TOKEN_EOF, "Expect end of expression.");

    endCompiler();

    return !parser.hadError;
}
