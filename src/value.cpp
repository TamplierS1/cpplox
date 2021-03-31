#include "value.h"

namespace cpplox
{
std::ostream& operator<<(std::ostream& stream, const Value& val)
{
    Val value = val.m_value;
    if (!value.has_value()) return stream << "nil";

    switch (value->index())
    {
        case 0:
            return stream << std::get<std::string>(value.value());
        case 1:
        {
            std::string text = std::to_string(std::get<double>(value.value()));
            text = trim_zeroes(text);
            return stream << text;
        }
        case 2:
        {
            bool boolean = std::get<bool>(value.value());
            if (boolean) return stream << "true";
            return stream << "false";
        }
        case 3:
        {
            // TODO: add support for `Callable`
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
