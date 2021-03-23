#include "value.h"

namespace garm
{

std::optional<std::string> literal_as_string(const Value& literal)
{
    auto value = literal.value();
    switch (value.index())
    {
        case 0:  // string
            return std::get<0>(value);
        case 1:  // double
            return std::to_string(std::get<1>(value));
        case std::variant_npos:  // nothing
            return std::nullopt;
        default:
            std::cout << "Error: not every type was handled.";
            return std::nullopt;
    }
}

}
