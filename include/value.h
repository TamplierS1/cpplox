#ifndef VALUE_H
#define VALUE_H

#include <optional>
#include <variant>

#include "error_handler.h"

namespace garm
{

// Value represents a literal value that some expressions can have - currently either a string or a double
using Value = std::optional<std::variant<std::string, double>>;

// Converts Value to string (it's used when printing tokens)
std::optional<std::string> literal_as_string(const Value& literal);

}

#endif  // VALUE_H