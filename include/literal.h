#include <optional>
#include <variant>

#include "error_handler.h"

namespace garm::types
{
using Literal = std::variant<std::string, double>;
using OptionalLiteral = std::optional<Literal>;

std::optional<std::string> literal_as_string(const Literal& literal);

}  // namespace garm::types
