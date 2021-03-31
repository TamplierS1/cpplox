#ifndef RETURN_H
#define RETURN_H

#include <exception>

#include "value.h"

namespace cpplox
{
// Represents a value returned from a function call
// shouldn't use exceptions like that >:(
class Return : public std::exception
{
public:
    explicit Return(const Value& value)
        : m_value(value)
    {}

    const Value m_value;
};

}

#endif // RETURN_H