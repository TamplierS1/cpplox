#ifndef LITERAL_H
#define LITERAL_H

#include <memory>

#include "expression.h"

namespace garm::ast
{
class Literal : public Expression
{
public:
    explicit Literal(const Value& value)
        : m_value(std::make_shared<Value>(value))
    {
    }

    Value accept(Visitor* visitor) override
    {
        return visitor->visit(this);
    }

    std::shared_ptr<Value> m_value;
};

}
#endif  // LITERAL_H