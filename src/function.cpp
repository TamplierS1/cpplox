#include "function.h"

#include "interpreter.h"

namespace cpplox
{
Value Function::call(Interpreter *interpreter, const std::vector<Value> &args)
{
    auto env = std::make_shared<Environment>(m_closure);

    for (int i = 0; i < m_declaration->m_params.size(); i++)
    {
        env->define(m_declaration->m_params.at(i).lexeme(), args.at(i));
    }

    // bad. bad. bad
    try
    {
        interpreter->execute_block(m_declaration->m_body, env);
    }
    catch (Return &return_value)
    {
        // allow using `return;` in initializers
        if (m_is_initializer)
            return m_closure->get_at(0, "this");

        return return_value.m_value;
    }

    // No return statement
    if (m_is_initializer)
        return m_closure->get_at(0, "this");

    return std::nullopt;
}

std::shared_ptr<Function> Function::bind(const std::shared_ptr<Instance> &instance)
{
    auto env = std::make_shared<Environment>(m_closure);
    env->define("this", instance);
    return std::make_shared<Function>(m_declaration, env, m_is_initializer, m_is_static);
}

}
