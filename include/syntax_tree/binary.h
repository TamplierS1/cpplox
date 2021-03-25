#ifndef BINARY_H
#define BINARY_H

#include <memory>

#include "expression.h"
#include "token.h"


namespace garm::ast
{
class Binary : public Expression
{
public:
    Binary(const ExpressionPtr& left, const garm::types::Token& op, const ExpressionPtr& right)
        : m_left(left)
        , m_op(std::make_shared<garm::types::Token>(op))
        , m_right(right)
    {
    }

    Value accept(Visitor* visitor) override
    {
        return visitor->visit(this);
    }

    ExpressionPtr m_left;
    std::shared_ptr<garm::types::Token> m_op;
    ExpressionPtr m_right;
};

}
#endif  // BINARY_H