#ifndef GROUPING_H
#define GROUPING_H

#include "expression.h"

namespace garm::ast
{
class Grouping : public Expression
{
public:
    explicit Grouping(Expression* expression)
        : m_expression(expression)
    {
    }

    Value accept(Visitor* visitor) override
    {
        return visitor->visit(this);
    }

    std::shared_ptr<Expression> m_expression;
};

}
#endif  // GROUPING_H