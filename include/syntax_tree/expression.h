#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "visitor.h"

namespace garm::ast
{
// Interface that every non-terminal in the grammar have to implement
class Expression
{
public:
    virtual ~Expression() = default;

    // Accepts a visitor that manipulates the data of this expression
    // The proper `visit` functions are deduced by dynamic dispatch
    virtual Value accept(Visitor* visitor) = 0;
};

}

#endif  // EXPRESSION_H