#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <memory>
#include <unordered_map>

#include "error_handler.h"
#include "value.h"

namespace cpplox
{
class Environment
{
public:
    // in this case m_enclosing remains uninitialized
    // and I will check for that later in get()
    Environment() = default;

    // local block scope
    explicit Environment(const std::shared_ptr<Environment>& enclosing)
    {
        m_enclosing = enclosing;
    }

    void define(const std::string& name, const Value& val);
    Value get(const Token& name);
    void assign(const Token& name, const Value& val);

    // outer scope
    std::weak_ptr<Environment> m_enclosing;

private:
    std::unordered_map<std::string, Value> m_values;
};

}

#endif  // ENVIRONMENT_H