#ifndef VALUE_H
#define VALUE_H

#include <optional>
#include <variant>
#include <iostream>

namespace cpplox
{

// Value represents a literal value that some expressions can have - currently either a string or a double
using Value = std::optional<std::variant<std::string, double, bool>>;

// Converts Value to string (it's used when printing tokens)
std::optional<std::string> literal_as_string(const Value& literal);

// Prints the value
std::ostream& operator<<(std::ostream& stream, const Value& val);

// Trims trailing zeroes from the end of the string
std::string trim_zeroes(const std::string& str);

}

#endif  // VALUE_H