#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <memory>

#include "token.h"

namespace cpplox::ast::expr
{
class Binary;
class Grouping;
class Literal;
class Unary;
class Variable;
class Assign;
class Logical;

// Interface that represents an operation executed on the given expressions
class Visitor
{
public:
    virtual Value visit(Binary* expr) = 0;
    virtual Value visit(Grouping* expr) = 0;
    virtual Value visit(Literal* expr) = 0;
    virtual Value visit(Unary* expr) = 0;
    virtual Value visit(Variable* expr) = 0;
    virtual Value visit(Assign* expr) = 0;
    virtual Value visit(Logical* expr) = 0;

protected:
    virtual ~Visitor() = default;
};

class Expression
{
public:
    virtual ~Expression() = default;

    // Accepts a visitor that manipulates the data of this expression
    // The proper `visit` functions are deduced by dynamic dispatch
    virtual Value accept(Visitor* visitor) = 0;
};

class Grouping : public Expression
{
public:
    explicit Grouping(const std::shared_ptr<Expression>& expression)
        : m_expression(expression)
    {
    }

    Value accept(Visitor* visitor) override
    {
        return visitor->visit(this);
    }

    std::shared_ptr<Expression> m_expression;
};

class Binary : public Expression
{
public:
    Binary(const std::shared_ptr<Expression>& left, const Token& op, const std::shared_ptr<Expression>& right)
        : m_left(left)
        , m_op(std::make_shared<Token>(op))
        , m_right(right)
    {
    }

    Value accept(Visitor* visitor) override
    {
        return visitor->visit(this);
    }

    std::shared_ptr<Expression> m_left;
    std::shared_ptr<Token> m_op;
    std::shared_ptr<Expression> m_right;
};

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

class Unary : public Expression
{
public:
    Unary(const Token& op, const std::shared_ptr<Expression>& right)
        : m_op(std::make_shared<Token>(op))
        , m_right(right)
    {
    }

    Value accept(Visitor* visitor) override
    {
        return visitor->visit(this);
    }

    std::shared_ptr<Token> m_op;
    std::shared_ptr<Expression> m_right;
};

class Variable : public Expression
{
public:
    explicit Variable(const Token& name)
        : m_name(std::make_shared<Token>(name))
    {
    }

    Value accept(Visitor* visitor) override
    {
        return visitor->visit(this);
    }

    std::shared_ptr<Token> m_name;
};

class Assign : public Expression
{
public:
    Assign(const Token& name, const std::shared_ptr<Expression>& value)
        : m_name(std::make_shared<Token>(name))
        , m_value(value)
    {
    }

    Value accept(Visitor* visitor) override
    {
        return visitor->visit(this);
    }

    std::shared_ptr<Token> m_name;
    std::shared_ptr<Expression> m_value;
};

class Logical : public Expression
{
public:
    Logical(const std::shared_ptr<Expression>& left, const Token& op, const std::shared_ptr<Expression>& right)
        : m_left(left)
        , m_op(std::make_shared<Token>(op))
        , m_right(right)
    {
    }

    Value accept(Visitor* visitor) override
    {
        return visitor->visit(this);
    }

    std::shared_ptr<Expression> m_left;
    std::shared_ptr<Token> m_op;
    std::shared_ptr<Expression> m_right;
};
}

using ExpressionPtr = std::shared_ptr<cpplox::ast::expr::Expression>;

#endif  // EXPRESSION_H