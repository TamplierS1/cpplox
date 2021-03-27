#include "environment.h"

namespace cpplox
{
void Environment::define(const std::string &name, const Value &val)
{
    // erase the previous value
    if (m_values.contains(name)) m_values.erase(name);

    m_values.insert({name, val});
}

Value Environment::get(const Token &name)
{
    auto element = m_values.find(name.get_lexeme());
    if (element != m_values.end())
        return element->second;

    // try outer scope
    if (!m_enclosing.expired())
        return m_enclosing.lock()->get(name);

    throw RuntimeError{name, "Undefined variable '" + name.get_lexeme() + "'."};
}

void Environment::assign(const Token &name, const Value &val)
{
    auto variable = m_values.find(name.get_lexeme());
    if(variable != m_values.end())
    {
        variable->second = val;
        return;
    }

    // try outer scope
    if (!m_enclosing.expired())
        m_enclosing.lock()->assign(name, val);

    throw RuntimeError{name, "Undefined variable '" + name.get_lexeme() + "'."};
}

}