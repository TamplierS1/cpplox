#include "instance.h"

#include "class.h"

namespace cpplox
{
Value Instance::get(const Token& name)
{
    if (m_fields.contains(name.lexeme()))
        return m_fields.at(name.lexeme());

    auto method = m_class->find_method(name.lexeme());
    if (method.has_value())
    {
        auto this_ptr = std::make_shared<Instance>(*this);
        return std::dynamic_pointer_cast<Callable>(method.value()->bind(this_ptr));
    }

    throw RuntimeError{name, "Undefined property '" + name.lexeme() + "'."};
}

void Instance::set(const Token& name, const Value& value)
{
    auto field = m_fields.find(name.lexeme());
    if (field != m_fields.end())
    {
        field->second = value;
    }
    else
    {
        // cpplox allows creating new fields on instances
        m_fields.emplace(name.lexeme(), value);
    }
}

std::string Instance::to_string() const
{
    return "<instance " + m_class->m_name + ">";
}

}