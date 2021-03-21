#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "visitor.h"

namespace garm::ast
{
class Expression
{
public:
    virtual ~Expression() = default;

    virtual Value accept(Visitor* visitor) = 0;
};

}

#endif  // EXPRESSION_H