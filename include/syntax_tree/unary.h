#ifndef UNARY_H
#define UNARY_H

#include <memory>

#include "expression.h"

namespace garm::ast
{
class Unary : public Expression
{
public:
Unary(const garm::types::Token& op, const ExpressionPtr& right)
: m_op(std::make_shared<garm::types::Token>(op)), m_right(right)
{}

Value accept(Visitor* visitor) override
{
return visitor->visit(this);
}

std::shared_ptr<garm::types::Token> m_op;
ExpressionPtr m_right;
};

}
#endif // UNARY_H