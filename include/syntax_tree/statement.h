#ifndef STATEMENT_H
#define STATEMENT_H

#include "expression.h"

namespace cpplox::ast::stmt
{
class Print;
class Expression;
class Var;
class Block;
class If;
class While;

// Interface that represents an operation executed on the given statements
class Visitor
{
public:
    // here `Expression` is an expression statement
    virtual void visit(Expression* stmt) = 0;
    virtual void visit(Print* stmt) = 0;
    virtual void visit(Var* stmt) = 0;
    virtual void visit(Block* stmt) = 0;
    virtual void visit(If* stmt) = 0;
    virtual void visit(While* stmt) = 0;

protected:
    virtual ~Visitor() = default;
};

class Statement
{
public:
    virtual ~Statement() = default;

    // Accepts a visitor that manipulates the data of this statement
    // The proper `visit` functions are deduced by dynamic dispatch
    virtual void accept(Visitor* visitor) = 0;
};

class Print : public Statement
{
public:
    explicit Print(const ExpressionPtr& expr)
        : m_expr(expr)
    {
    }

    void accept(Visitor* visitor) override
    {
        return visitor->visit(this);
    }

    std::shared_ptr<expr::Expression> m_expr;
};

class Expression : public Statement
{
public:
    explicit Expression(const ExpressionPtr& expr)
        : m_expr(expr)
    {
    }

    void accept(Visitor* visitor) override
    {
        return visitor->visit(this);
    }

    std::shared_ptr<expr::Expression> m_expr;
};

class Var : public Statement
{
public:
    // A variable may be not initialized,
    // so it takes std::optional to indicate that.
    // If a variable is not initialized just pass std::nullopt
    Var(const Token& name, const std::optional<std::shared_ptr<expr::Expression>>& initializer)
        : m_name(std::make_shared<Token>(name))
        , m_initializer(initializer)
    {
    }

    void accept(Visitor* visitor) override
    {
        return visitor->visit(this);
    }

    std::shared_ptr<Token> m_name;
    std::optional<std::shared_ptr<expr::Expression>> m_initializer;
};

class Block : public Statement
{
public:
    explicit Block(const std::vector<std::shared_ptr<Statement>>& stmts)
        : m_statements(stmts)
    {
    }

    void accept(Visitor* visitor) override
    {
        return visitor->visit(this);
    }

    std::vector<std::shared_ptr<Statement>> m_statements;
};

class If : public Statement
{
public:
    If(const ExpressionPtr& condition, const std::shared_ptr<Statement>& then,
       std::optional<std::shared_ptr<Statement>>& else_branch)
        : m_condition(condition)
        , m_then(then)
        , m_else(else_branch)
    {
    }

    void accept(Visitor* visitor) override
    {
        return visitor->visit(this);
    }

    ExpressionPtr m_condition;
    std::shared_ptr<Statement> m_then;
    std::optional<std::shared_ptr<Statement>> m_else;
};

class While : public Statement
{
public:
    While(const ExpressionPtr& condition, const std::shared_ptr<Statement>& stmt)
        : m_condition(condition)
        , m_stmt(stmt)
    {
    }

    void accept(Visitor* visitor) override
    {
        return visitor->visit(this);
    }

    ExpressionPtr m_condition;
    std::shared_ptr<Statement> m_stmt;
};

}

using StatementPtr = std::shared_ptr<cpplox::ast::stmt::Statement>;

#endif  // STATEMENT_H