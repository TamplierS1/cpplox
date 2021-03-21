#ifndef UNARY_H
#define UNARY_H

#include "expression.h"

namespace garm::ast
{
class Unary : public Expression
{
public:
Unary(garm::types::Token* op, Expression* right)
: m_op(op), m_right(right)
{}

Value accept(Visitor* visitor) override
{
return visitor->visit(this);
}

std::shared_ptr<garm::types::Token> m_op;
std::shared_ptr<Expression> m_right;
};

}
#endif // UNARY_H