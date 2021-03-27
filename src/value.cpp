#include "value.h"

namespace cpplox
{
std::optional<std::string> literal_as_string(const Value& literal)
{
    auto value = literal.value();
    switch (value.index())
    {
        case 0:
            return std::get<std::string>(value);
        case 1:
            return std::to_string(std::get<double>(value));
        case 2:
        {
            bool val = std::get<bool>(value);
            if (val) return "true";
            return "false";
        }
        case std::variant_npos:  // nothing
            return std::nullopt;
        default:
            std::cout << "Error: not every type was handled.";
            return std::nullopt;
    }
}

std::ostream& operator<<(std::ostream& stream, const Value& val)
{
    if (!val.has_value())
        return stream << "nil";

    switch (val->index())
    {
        case 0:
            return stream << std::get<std::string>(val.value());
        case 1:
        {
            std::string text = std::to_string(std::get<double>(val.value()));
            text = trim_zeroes(text);
            return stream << text;
        }
        case 2:
        {
            bool boolean = std::get<bool>(val.value());
            if (boolean) return stream << "true";
            return stream << "false";
        }
        case std::variant_npos:
            return stream << "nil";
        default:
            std::cout << "Error: not every type was handled.";
            return stream;
    }
}

std::string trim_zeroes(const std::string& str)
{
    if (str.ends_with(".000000"))
    {
        return str.substr(0, str.size() - 7);
    }

    return str;
}

}
