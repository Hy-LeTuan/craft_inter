#include <stmt.hpp>
#include <token.hpp>

#include <any>

namespace stmt
{

Stmt::~Stmt()
{
}

Expression::Expression(Expr* expression)
  : expression{ expression }
{
}

Expression::~Expression()
{
    if (expression)
    {
        delete expression;
    }
}

std::any Expression::accept(Visitor* visitor) const
{
    return visitor->visitExpressionStmt(this);
}

If::If(Expr* condition, Stmt* thenBranch, Stmt* elseBranch)
  : condition{ condition }
  , thenBranch{ thenBranch }
  , elseBranch{ elseBranch }
{
}

If::~If()
{
    if (condition)
    {
        delete condition;
    }
    if (thenBranch)
    {
        delete thenBranch;
    }
    if (elseBranch)
    {
        delete elseBranch;
    }
}

std::any If::accept(Visitor* visitor) const
{
    return visitor->visitIfStmt(this);
}

Function::Function(Token* name, vector<Token*>* params, vector<Stmt*>* body)
  : name{ name }
  , params{ params }
  , body{ body }
{
}

Function::~Function()
{
    if (name)
    {
        delete name;
    }
    if (params)
    {
        delete params;
    }
    if (body)
    {
        delete body;
    }
}

std::any Function::accept(Visitor* visitor) const
{
    return visitor->visitFunctionStmt(this);
}

Block::Block(vector<Stmt*>* statements)
  : statements{ statements }
{
}

Block::~Block()
{
    if (statements)
    {
        delete statements;
    }
}

std::any Block::accept(Visitor* visitor) const
{
    return visitor->visitBlockStmt(this);
}

Class::Class(Token* name, Variable* superclass, vector<Stmt*>* methods)
  : name{ name }
  , superclass{ superclass }
  , methods{ methods }
{
}

Class::~Class()
{
    if (name)
    {
        delete name;
    }
    if (superclass)
    {
        delete superclass;
    }
    if (methods)
    {
        delete methods;
    }
}

std::any Class::accept(Visitor* visitor) const
{
    return visitor->visitClassStmt(this);
}

Print::Print(Expr* expression)
  : expression{ expression }
{
}

Print::~Print()
{
    if (expression)
    {
        delete expression;
    }
}

std::any Print::accept(Visitor* visitor) const
{
    return visitor->visitPrintStmt(this);
}

Return::Return(Token* keyword, Expr* value)
  : keyword{ keyword }
  , value{ value }
{
}

Return::~Return()
{
    if (keyword)
    {
        delete keyword;
    }
    if (value)
    {
        delete value;
    }
}

std::any Return::accept(Visitor* visitor) const
{
    return visitor->visitReturnStmt(this);
}

Var::Var(Token* name, Expr* initializer)
  : name{ name }
  , initializer{ initializer }
{
}

Var::~Var()
{
    if (name)
    {
        delete name;
    }
    if (initializer)
    {
        delete initializer;
    }
}

std::any Var::accept(Visitor* visitor) const
{
    return visitor->visitVarStmt(this);
}

While::While(Expr* condition, Stmt* body)
  : condition{ condition }
  , body{ body }
{
}

While::~While()
{
    if (condition)
    {
        delete condition;
    }
    if (body)
    {
        delete body;
    }
}

std::any While::accept(Visitor* visitor) const
{
    return visitor->visitWhileStmt(this);
}

}
