#ifndef GROUPING_H
#define GROUPING_H

#include <memory>

#include "expression.h"

namespace garm::ast
{
class Grouping : public Expression
{
public:
    explicit Grouping(const ExpressionPtr& expression)
        : m_expression(expression)
    {
    }

    Value accept(Visitor* visitor) override
    {
        return visitor->visit(this);
    }

    ExpressionPtr m_expression;
};

}
#endif  // GROUPING_H