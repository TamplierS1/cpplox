#include "function.h"
#include "interpreter.h"

namespace cpplox
{
Value Function::call(Interpreter *interpreter, const std::vector<Value> &args)
{
    auto env = std::make_shared<Environment>(m_closure);

    for (int i = 0; i < m_declaration->m_params.size(); i++)
    {
        env->define(m_declaration->m_params.at(i)->get_lexeme(), args.at(i));
    }

    // bad. bad. bad
    try
    {
        interpreter->execute_block(m_declaration->m_body, env);
    }
    catch(Return& return_value)
    {
        return return_value.m_value;
    }
    return std::nullopt;
}

unsigned int Function::arity() const
{
    return m_declaration->m_params.size();
}

std::string Function::to_string() const
{
    return "<fn " + m_declaration->m_name->get_lexeme();
}

}
