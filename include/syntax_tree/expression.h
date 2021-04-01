#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <memory>

#include "token.h"

namespace cpplox::ast::stmt
{
// this is used in `Lambda`
class Statement;
}

namespace cpplox::ast::expr
{
class Binary;
class Grouping;
class Literal;
class Unary;
class Variable;
class Assign;
class Logical;
class Call;
class Lambda;

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
    virtual Value visit(Call* expr) = 0;
    virtual Value visit(Lambda* expr) = 0;

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

class Call : public Expression
{
public:
    Call(const std::shared_ptr<Expression>& callee, const Token& paren,
         const std::vector<std::shared_ptr<Expression>>& args)
        : m_callee(callee)
        , m_paren(std::make_shared<Token>(paren))
        , m_args(args)
    {
    }

    Value accept(Visitor* visitor) override
    {
        return visitor->visit(this);
    }

    // callee is the thing being called
    std::shared_ptr<Expression> m_callee;
    std::shared_ptr<Token> m_paren;
    std::vector<std::shared_ptr<Expression>> m_args;
};

class Lambda : public Expression
{
public:
    Lambda(const std::vector<std::shared_ptr<Token>>& params, const std::vector<std::shared_ptr<cpplox::ast::stmt::Statement>>& body)
        : m_params(params)
        , m_body(body)
    {
    }

    Value accept(Visitor* visitor) override
    {
        return visitor->visit(this);
    }

    std::vector<std::shared_ptr<Token>> m_params;
    std::vector<std::shared_ptr<cpplox::ast::stmt::Statement>> m_body;
};

}

using ExpressionPtr = std::shared_ptr<cpplox::ast::expr::Expression>;

#endif  // EXPRESSION_H