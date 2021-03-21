#ifndef VALUE_H
#define VALUE_H

#include <optional>
#include <variant>

#include "error_handler.h"

namespace garm
{
using Value = std::optional<std::variant<std::string, double>>;

std::optional<std::string> literal_as_string(const Value& literal);

}

#endif  // VALUE_H