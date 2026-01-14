#include <lox_function.hpp>

#include <return.hpp>

LoxFunction::LoxFunction(
  const stmt::Function* declaration, Environment* closure, bool isInitializer)
  : declaration{ declaration }
  , closure{ closure }
  , isInitializer{ isInitializer }
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
        if (isInitializer)
        {
            return closure->getAt(0, "this");
        }

        return returnValue.getValue();
    }

    if (isInitializer)
    {
        return closure->getAt(0, "this");
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

LoxFunction* LoxFunction::bind(const LoxInstance* instance)
{
    Environment* environment = new Environment{ closure };
    environment->define("this", const_cast<LoxInstance*>(instance));

    return new LoxFunction(declaration, environment, isInitializer);
}
