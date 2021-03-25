#ifndef VALUE_H
#define VALUE_H

#include <optional>
#include <variant>
#include <iostream>

namespace garm
{

// Value represents a literal value that some expressions can have - currently either a string or a double
using Value = std::optional<std::variant<std::string, double, bool>>;

// Converts Value to string (it's used when printing tokens)
std::optional<std::string> literal_as_string(const Value& literal);

std::ostream& operator<<(std::ostream& stream, const Value& val);
}

#endif  // VALUE_H