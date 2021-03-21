#ifndef BINARY_H
#define BINARY_H

#include "expression.h"
#include "token.h"

namespace garm::ast
{
class Binary : public Expression
{
public:
    Binary(Expression* left, garm::types::Token* op, Expression* right)
        : m_left(left)
        , m_op(op)
        , m_right(right)
    {
    }

    Value accept(Visitor* visitor) override
    {
        return visitor->visit(this);
    }

    std::shared_ptr<Expression> m_left;
    std::shared_ptr<garm::types::Token> m_op;
    std::shared_ptr<Expression> m_right;
};

}
#endif  // BINARY_H