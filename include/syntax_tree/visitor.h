#ifndef VISITOR_H
#define VISITOR_H

#include "value.h"

namespace garm::ast
{
class Binary;
class Grouping;
class Literal;
class Unary;

// Interface that represents an operation executed on the given expressions
class Visitor
{
public:
    virtual Value visit(Binary* expr) = 0;
    virtual Value visit(Grouping* expr) = 0;
    virtual Value visit(Literal* expr) = 0;
    virtual Value visit(Unary* expr) = 0;
};
}

#endif  // VISITOR_H
