#include "literal.h"

namespace garm::types
{

std::optional<std::string> literal_as_string(const Literal& literal)
{
    switch (literal.index())
    {
        case 0: //string
            return std::get<0>(literal);
        case 1: //double
            return std::to_string(std::get<1>(literal));
        case std::variant_npos: // nothing
            return std::nullopt;
        default:
            ErrorHandler::get_instance().debug_error("Not every type was handled");
    }
}

}