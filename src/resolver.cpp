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
    if (stmt->m_else.has_value())
        resolve(stmt->m_else->get());
}

void Resolver::visit(stmt::Print *stmt)
{
    resolve(stmt->m_expr.get());
}

void Resolver::visit(stmt::Return *stmt)
{
    if (m_current_func == FunctionType::NONE)
        error(stmt->m_keyword, "Can't return from top-level code.");

    if (stmt->m_value.has_value())
    {
        if (m_current_func == FunctionType::INITIALIZER)
            error(stmt->m_keyword, "Can't return a value from an initializer.");

        resolve(stmt->m_value->get());
    }
}

void Resolver::visit(stmt::While *stmt)
{
    resolve(stmt->m_condition.get());
    resolve(stmt->m_stmt.get());
}

void Resolver::visit(stmt::Class *stmt)
{
    ClassType enclosing_class = m_current_class;
    m_current_class = ClassType::CLASS;

    declare(stmt->m_name);
    define(stmt->m_name);

    bool super_exists = stmt->m_super.has_value();

    if (super_exists && stmt->m_name.lexeme() == stmt->m_super.value()->m_name.lexeme())
    {
        error(stmt->m_super.value()->m_name, "A class can't inherit itself.");
    }

    if (super_exists)
    {
        m_current_class = ClassType::SUBCLASS;

        resolve(stmt->m_super->get());

        begin_scope();
        m_scopes.back().emplace("super", true);
    }

    begin_scope();
    m_scopes.back().emplace("this", true);

    for (const auto &method : stmt->m_methods)
    {
        FunctionType declaration = FunctionType::METHOD;
        if (method->m_name.lexeme() == "init")
            declaration = FunctionType::INITIALIZER;

        resolve_function(method.get(), declaration);
    }

    end_scope();
    if (super_exists)
        end_scope();

    m_current_class = enclosing_class;
}

void Resolver::visit(stmt::Import *stmt)
{
    import_module(stmt->m_module);
}

Value Resolver::visit(expr::Variable *expr)
{
    if (!m_scopes.empty())
    {
        auto is_ready = m_scopes.back().find(expr->m_name.lexeme());
        if (is_ready != m_scopes.back().end() && !is_ready->second)
        {
            error(expr->m_name, "Can't read local variable in its own initializer.");
        }
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
    FunctionType enclosing_func = m_current_func;
    m_current_func = FunctionType::FUNCTION;

    begin_scope();
    for (const Token &param : expr->m_params)
    {
        declare(param);
        define(param);
    }

    resolve(expr->m_body);

    end_scope();

    m_current_func = enclosing_func;

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

Value Resolver::visit(expr::Get *expr)
{
    resolve(expr->m_object.get());

    return std::nullopt;
}

Value Resolver::visit(expr::Set *expr)
{
    resolve(expr->m_value.get());
    resolve(expr->m_object.get());

    return std::nullopt;
}

Value Resolver::visit(expr::This *expr)
{
    if (m_current_class == ClassType::NONE)
    {
        error(expr->m_keyword, "Can't use 'this' outside of a class.");
        return std::nullopt;
    }

    resolve_local(expr, expr->m_keyword);

    return std::nullopt;
}

Value Resolver::visit(expr::Super *expr)
{
    if (m_current_class != ClassType::SUBCLASS)
    {
        error(expr->m_keyword, "Can't use 'super' when there is no super class.");
        return std::nullopt;
    }

    resolve_local(expr, expr->m_keyword);

    return std::nullopt;
}

void Resolver::resolve(const std::vector<StatementPtr> &stmts)
{
    for (const auto &stmt : stmts)
    {
        try
        {
            resolve(stmt.get());
        }
        catch(RuntimeError& e)
        {
            error(e.m_op, e.m_msg);
            break;
        }
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
        if (scope.contains(name.lexeme()))
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

    check_prefixes(function, type);

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
    if (m_scopes.empty())
        return;

    auto &scope = m_scopes.back();
    if (scope.contains(name.lexeme()))
    {
        error(name, "Variable with this name is already declared in this scope.");
    }

    bool is_ready = false;
    scope.emplace(name.lexeme(), is_ready);
}

void Resolver::define(const Token &name)
{
    // the resolver skips any global variable declarations
    if (m_scopes.empty())
        return;

    bool is_ready = true;
    m_scopes.back().at(name.lexeme()) = is_ready;
}

void Resolver::import_module(const Token &name)
{
    // Cpplox does not allow importing the main script anywhere
    if (name == m_filename)
    {
        throw RuntimeError{name, "Can't import the main script as it is not a module."};
    }

    if (is_imported(name.lexeme()))
    {
        error(name, "The module '" + name.lexeme() + "' was already imported.");
        return;
    }

    for (const std::string &path : m_search_paths)
    {
        Scanner scanner;
        auto tokens = scanner.run_file(path + "/" + name.lexeme() + ".cpplox");

        Parser parser{tokens.value()};
        std::optional<std::vector<StatementPtr>> statements = parser.parse();
        // TODO: turn return codes into enums
        if (ErrorHandler::get_instance().m_had_error)
            std::exit(65);

        // We store the modules' name here to prevent circular dependency that would
        // make our interpreter crash
        m_imported_modules.push_back(name.lexeme());

        resolve(statements.value());
        if (ErrorHandler::get_instance().m_had_error)
            std::exit(65);

        m_interpreter.lock()->add_statements(statements.value());

        return;
    }
}

bool Resolver::is_imported(const std::string &name)
{
    return std::find(m_imported_modules.begin(), m_imported_modules.end(), name) != m_imported_modules.end();
}

void Resolver::check_prefixes(stmt::Function *function, FunctionType type)
{
    // TODO: extend this method when you add new prefixes
    if (type == FunctionType::FUNCTION)
    {
        for (const auto &prefix : function->m_prefix)
        {
            if (prefix.lexeme() == "static")
                error(prefix, "Only methods can be declared static.");
        }
    }
}

void Resolver::error(const Token &name, const std::string& msg)
{
    ErrorHandler::get_instance().error(name, msg);
}

void Resolver::warning(const Token &name, const std::string &msg)
{
    ErrorHandler::get_instance().warning(name, msg);
}

}
