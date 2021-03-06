#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <memory>
#include <optional>
#include <variant>

#include "callable.h"

namespace cpplox
{
class Instance;

using Val =
    std::optional<std::variant<std::string, double, bool, std::shared_ptr<Callable>, std::shared_ptr<Instance>>>;

class Value
{
public:
    // don't make constructors explicit - we need implicit conversion (except for bools and doubles)
    Value(std::nullopt_t)
        : m_value(std::nullopt)
    {
    }
    Value(const std::string& value)
        : m_value(value)
    {
    }
    explicit Value(const double& value)
        : m_value(value)
    {
    }
    explicit Value(const bool& value)
        : m_value(value)
    {
    }
    Value(const std::shared_ptr<Callable>& value)
        : m_value(value)
    {
    }
    Value(const std::shared_ptr<Instance>& value)
        : m_value(value)
    {
    }

    // Prints the value
    friend std::ostream& operator<<(std::ostream& stream, const Value& val);
    [[nodiscard]] std::string to_string() const;

    Val m_value;
};

// Trims trailing zeroes from the end of the string
std::string trim_zeroes(const std::string& str);

}

#endif  // VALUE_H