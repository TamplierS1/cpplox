#include "environment.h"

namespace cpplox
{
void Environment::define(const std::string &name, const Value &val)
{
    // erase the previous value
    if (m_values.contains(name)) m_values.erase(name);

    m_values.insert({name, val});
}

Value Environment::get(const Token &name) const
{
    auto element = m_values.find(name.lexeme());
    if (element != m_values.end())
        return element->second;

    // try outer scope
    if (m_enclosing != nullptr)
        return m_enclosing->get(name);

    throw RuntimeError{name, "Undefined variable '" + name.lexeme() + "'."};
}

Value Environment::get_at(int distance, const std::string &name)
{
    return ancestor(distance)->m_values.at(name);
}

void Environment::assign(const Token &name, const Value &val)
{
    auto variable = m_values.find(name.lexeme());
    if (variable != m_values.end())
    {
        variable->second = val;
        return;
    }

    // try outer scope
    if (m_enclosing != nullptr)
    {
        m_enclosing->assign(name, val);
        return;
    }

    throw RuntimeError{name, "Undefined variable '" + name.lexeme() + "'."};
}

void Environment::assign_at(int distance, const Token &name, const Value &val)
{
    ancestor(distance)->m_values.at(name.lexeme()) = val;
}

std::shared_ptr<Environment> Environment::ancestor(int distance) const
{
    auto env = std::make_shared<Environment>(*this);
    for (int i = 0; i < distance; i++)
    {
        env = env->m_enclosing;
    }

    return env;
}

}