#include "value.h"

#include "error_handler.h"
#include "instance.h"

namespace cpplox
{
std::ostream& operator<<(std::ostream& stream, const Value& val)
{
    return stream << val.to_string();
}

std::string Value::to_string() const
{
    if (!m_value.has_value()) return "nil";

    switch (m_value->index())
    {
        case 0:
            return std::get<std::string>(m_value.value());
        case 1:
        {
            std::string text = std::to_string(std::get<double>(m_value.value()));
            text = trim_zeroes(text);
            return text;
        }
        case 2:
        {
            bool boolean = std::get<bool>(m_value.value());
            if (boolean) return "true";
            return "false";
        }
        case 3:
        {
            return std::get<std::shared_ptr<Callable>>(m_value.value())->to_string();
        }
        case 4:
        {
            return std::get<std::shared_ptr<Instance>>(m_value.value())->to_string();
        }
        case std::variant_npos:
            return "nil";
        default:
            ErrorHandler::get_instance().debug_error("Error: not every type was handled.", __LINE__);
            return "";
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
