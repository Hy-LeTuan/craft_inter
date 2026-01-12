#include <return.hpp>
#include <lox_function.hpp>

LoxFunction::LoxFunction(const stmt::Function* declaration, Environment* closure)
  : declaration{ declaration }
  , closure{ closure }
{
}

LoxFunction::~LoxFunction()
{
    if (declaration)
    {
        delete declaration;
    }
}

Object LoxFunction::call(Interpreter* interpreter, std::vector<Object>* arguments)
{
    Environment* environment = new Environment(closure);

    for (int i = 0; i < declaration->params->size(); i++)
    {
        environment->define(declaration->params->at(i)->getLexeme(), arguments->at(i));
    }

    try
    {
        interpreter->executeBlock(declaration->body, environment);
    }
    catch (Return returnValue)
    {
        return returnValue.getValue();
    }

    return nullptr;
}

int LoxFunction::arity()
{
    return declaration->params->size();
}

std::string LoxFunction::toString()
{
    return "<fn" + declaration->name->getLexeme() + ">";
}
