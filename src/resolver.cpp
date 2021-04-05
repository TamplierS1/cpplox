#include "resolver.h"

namespace cpplox
{
void Resolver::visit(stmt::Block *stmt)
{
    begin_scope();
    resolve(stmt->m_statements);
    end_scope();
}

void Resolver::visit(stmt::Var *stmt)
{
    declare(stmt->m_name);
    if (stmt->m_initializer.has_value())
    {
        resolve(stmt->m_initializer->get());
    }
    define(stmt->m_name);
}

void Resolver::visit(stmt::Function *stmt)
{
    // we declare and immediately define the function,
    // because it allows recursion
    declare(stmt->m_name);
    define(stmt->m_name);

    resolve_function(stmt, FunctionType::FUNCTION);
}

void Resolver::visit(stmt::Expression *stmt)
{
    resolve(stmt->m_expr.get());
}

void Resolver::visit(stmt::If *stmt)
{
    resolve(stmt->m_condition.get());
    resolve(stmt->m_then.get());
    if (stmt->m_else.has_value()) resolve(stmt->m_else->get());
}

void Resolver::visit(stmt::Print *stmt)
{
    resolve(stmt->m_expr.get());
}

void Resolver::visit(stmt::Return *stmt)
{
    if (m_current_func == FunctionType::NONE)
        ErrorHandler::get_instance().error(stmt->m_keyword, "Can't return from top-level code.");

    if (stmt->m_value.has_value()) resolve(stmt->m_value->get());
}

void Resolver::visit(stmt::While *stmt)
{
    resolve(stmt->m_condition.get());
    resolve(stmt->m_stmt.get());
}

Value Resolver::visit(expr::Variable *expr)
{
    if (m_scopes.empty())  return std::nullopt;

    auto is_ready = m_scopes.back().find(expr->m_name.get_lexeme());
    if (is_ready != m_scopes.back().end() && !is_ready->second)
    {
        cpplox::ErrorHandler::get_instance().error(expr->m_name, "Can't read local variable in its own initializer.");
    }

    resolve_local(expr, expr->m_name);

    return std::nullopt;
}

Value Resolver::visit(expr::Assign *expr)
{
    // resolve any variables that the assignment might contain
    resolve(expr->m_value.get());
    // resolve the variable that's being assigned to
    resolve_local(expr, expr->m_name);
    return std::nullopt;
}

Value Resolver::visit(expr::Lambda *expr)
{
    begin_scope();
    for (const Token &param : expr->m_params)
    {
        declare(param);
        define(param);
    }

    resolve(expr->m_body);

    end_scope();

    return std::nullopt;
}

Value Resolver::visit(expr::Binary *expr)
{
    resolve(expr->m_left.get());
    resolve(expr->m_right.get());

    return std::nullopt;
}

Value Resolver::visit(expr::Call *expr)
{
    resolve(expr->m_callee.get());

    for (const auto &arg : expr->m_args)
    {
        resolve(arg.get());
    }

    return std::nullopt;
}

Value Resolver::visit(expr::Grouping *expr)
{
    resolve(expr->m_expression.get());

    return std::nullopt;
}

Value Resolver::visit(expr::Literal *expr)
{
    // there is nothing to resolve in a literal
    return std::nullopt;
}

Value Resolver::visit(expr::Logical *expr)
{
    resolve(expr->m_left.get());
    resolve(expr->m_right.get());

    return std::nullopt;
}

Value Resolver::visit(expr::Unary *expr)
{
    resolve(expr->m_right.get());

    return std::nullopt;
}

void Resolver::resolve(const std::vector<StatementPtr> &stmts)
{
    for (const auto &stmt : stmts)
    {
        resolve(stmt.get());
    }
}

void Resolver::resolve(stmt::Statement *stmt)
{
    stmt->accept(this);
}

void Resolver::resolve(expr::Expression *expr)
{
    expr->accept(this);
}

void Resolver::resolve_local(expr::Expression *expr, const Token &name)
{
    // how many scopes we had to traverse before we found the variable
    int num_scopes = 0;
    // we start iterating from the end,
    // because we need to start from the innermost scope and continue outwards.
    // if we don't find the variable we assume it's global
    std::for_each(m_scopes.rbegin(), m_scopes.rend(), [&](const auto &scope) -> void {
        if (scope.contains(name.get_lexeme()))
        {
            m_interpreter.lock()->resolve(expr, num_scopes);
            return;
        }
        num_scopes++;
    });
}

void Resolver::resolve_function(stmt::Function *function, FunctionType type)
{
    FunctionType enclosing_func = m_current_func;
    m_current_func = type;

    begin_scope();
    for (const Token &param : function->m_params)
    {
        declare(param);
        define(param);
    }

    resolve(function->m_body);

    end_scope();

    m_current_func = enclosing_func;
}

void Resolver::begin_scope()
{
    m_scopes.emplace_back(Scope{});
}

void Resolver::end_scope()
{
    m_scopes.pop_back();
}

void Resolver::declare(const Token &name)
{
    // the resolver skips any global variable declarations
    if (m_scopes.empty()) return;

    auto& scope = m_scopes.back();
    if (scope.contains(name.get_lexeme()))
    {
        cpplox::ErrorHandler::get_instance().error(name, "Variable with this name is already declared in this scope.");
    }

    bool is_ready = false;
    scope.emplace(name.get_lexeme(), is_ready);
}

void Resolver::define(const Token &name)
{
    // the resolver skips any global variable declarations
    if (m_scopes.empty()) return;

    bool is_ready = true;
    m_scopes.back().at(name.get_lexeme()) = is_ready;
}

}
