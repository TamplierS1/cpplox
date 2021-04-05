#include "interpreter.h"

namespace cpplox
{
Interpreter::Interpreter()
{
    register_native_funcs();
}

Interpreter::~Interpreter() noexcept
{
    for (auto &expr : m_locals)
    {
        /* TODO: this causes a double free and crashes the interpreter.
         *  Probably because I got those pointers from `get()` method of a shared pointer,
         *  that is freed automatically. So even if I don't free the pointers in this class
         *  they're going to get freed anyway.
         */
        // delete expr.first;
    }
}

void Interpreter::interpret(std::vector<StatementPtr> &stmts)
{
    try
    {
        for (auto &stmt : stmts)
        {
            execute(stmt.get());
        }
    }
    catch (RuntimeError &e)
    {
        ErrorHandler::get_instance().runtime_error(e);
    }
}

Value Interpreter::visit(expr::Literal *expr)
{
    return *expr->m_value;
}

Value Interpreter::visit(expr::Grouping *expr)
{
    return evaluate(expr->m_expression.get());
}

Value Interpreter::visit(expr::Unary *expr)
{
    Value right = evaluate(expr->m_right.get());

    switch (expr->m_op.get_token_type())
    {
        case TokenType::MINUS:
            check_number_operands(expr->m_op, right);
            // a unary minus can only be applied to numbers and that's why we convert the expression to double
            return -std::get<double>(right.m_value.value());
        case TokenType::BANG:
            return !is_true(right);
    }

    return std::nullopt;
}

Value Interpreter::visit(expr::Binary *expr)
{
    Value left = evaluate(expr->m_left.get());
    Value right = evaluate(expr->m_right.get());

    // extract the values beforehand to avoid repetition
    double dleft = 0, dright = 0;
    bool is_numbers = false;
    // numbers
    if (std::holds_alternative<double>(left.m_value.value()) && std::holds_alternative<double>(right.m_value.value()))
    {
        dleft = std::get<double>(left.m_value.value());
        dright = std::get<double>(right.m_value.value());
        is_numbers = true;
    }

    switch (expr->m_op.get_token_type())
    {
            /* Arithmetic */
        case TokenType::MINUS:
            check_number_operands(expr->m_op, left, right);
            return dleft - dright;
        case TokenType::PLUS:
            if (is_numbers) return dleft + dright;

            if (std::holds_alternative<std::string>(left.m_value.value()) ||
                std::holds_alternative<std::string>(right.m_value.value()))
            {
                return left.to_string() + right.to_string();
            }

        case TokenType::SLASH:
            check_number_operands(expr->m_op, left, right);
            if (dright == 0) throw RuntimeError{expr->m_op, "Cannot divide by zero."};
            return dleft / dright;
        case TokenType::STAR:
            check_number_operands(expr->m_op, left, right);
            return dleft * dright;
            /* Comparison */
        case TokenType::GREATER:
            check_number_operands(expr->m_op, left, right);
            return dleft > dright;
        case TokenType::GREATER_EQUAL:
            check_number_operands(expr->m_op, left, right);
            return dleft >= dright;
        case TokenType::LESS:
            check_number_operands(expr->m_op, left, right);
            return dleft < dright;
        case TokenType::LESS_EQUAL:
            check_number_operands(expr->m_op, left, right);
            return dleft <= dright;
        case TokenType::BANG_EQUAL:
            return !is_equal(left, right);
        case TokenType::EQUAL_EQUAL:
            return is_equal(left, right);
    }

    return std::nullopt;
}

Value Interpreter::visit(expr::Variable *expr)
{
    return lookup_variable(expr->m_name, expr);
}

Value Interpreter::visit(expr::Assign *expr)
{
    Value val = evaluate(expr->m_value.get());

    auto distance = m_locals.find(expr);
    if (distance != m_locals.end())
    {
        m_env->assign_at(distance->second, expr->m_name, val);
    }
    else
        m_globals->assign(expr->m_name, val);

    return val;
}

Value Interpreter::visit(expr::Logical *expr)
{
    Value left = evaluate(expr->m_left.get());

    if (expr->m_op.get_token_type() == TokenType::OR)
    {
        if (is_true(left)) return left;
    }
    else
    {
        if (!is_true(left)) return left;
    }

    return evaluate(expr->m_right.get());
}

Value Interpreter::visit(expr::Call *expr)
{
    Value callee = evaluate(expr->m_callee.get());

    std::vector<Value> args;
    for (const auto &arg : expr->m_args)
    {
        args.emplace_back(evaluate(arg.get()));
    }

    // check if the `callee` is actually something we can call
    if (dynamic_cast<Callable *>(std::get<std::shared_ptr<Callable>>(callee.m_value.value()).get()) == nullptr)
    {
        throw RuntimeError{expr->m_paren, "Can only call functions and classes."};
    }

    auto function = std::get<std::shared_ptr<Callable>>(callee.m_value.value());

    // check for right number of arguments
    if (args.size() != function->arity())
    {
        throw RuntimeError{expr->m_paren, "Expected " + std::to_string(function->arity()) + " arguments, but got " +
                                              std::to_string(args.size()) + "."};
    }

    return function->call(this, args);
}

Value Interpreter::visit(expr::Lambda *expr)
{
    auto lambda_expr = std::make_shared<expr::Lambda>(*expr);
    auto lambda = std::make_shared<Lambda>(lambda_expr);
    return std::dynamic_pointer_cast<Callable>(lambda);
}

void Interpreter::visit(stmt::Expression *stmt)
{
    evaluate(stmt->m_expr.get());
}

void Interpreter::visit(stmt::Print *stmt)
{
    Value value = evaluate(stmt->m_expr.get());
    // TODO: provide formatter<T> specialization, which will allow me to use fmt::print
    std::cout << value;
}

void Interpreter::visit(stmt::Var *stmt)
{
    Value value = std::nullopt;
    if (stmt->m_initializer.has_value())
    {
        value = evaluate(stmt->m_initializer->get());
    }

    m_env->define(stmt->m_name.get_lexeme(), value);
}

void Interpreter::visit(stmt::Block *stmt)
{
    /* TODO: there is a leak here. I throw an exception in `execute_block()`
        and we never get back here to free `env`
    */
    auto env = std::make_shared<Environment>(m_env);
    execute_block(stmt->m_statements, env);
}

void Interpreter::visit(stmt::If *stmt)
{
    if (is_true(evaluate(stmt->m_condition.get())))
    {
        execute(stmt->m_then.get());
    }
    else if (stmt->m_else.has_value())
    {
        execute(stmt->m_else->get());
    }
}

void Interpreter::visit(stmt::While *stmt)
{
    while (is_true(evaluate(stmt->m_condition.get())))
    {
        execute(stmt->m_stmt.get());
    }
}

void Interpreter::visit(stmt::Function *stmt)
{
    auto statement = std::make_shared<stmt::Function>(*stmt);
    auto function = std::make_shared<Function>(statement, m_env);
    m_env->define(stmt->m_name.get_lexeme(), std::dynamic_pointer_cast<Callable>(function));
}

void Interpreter::visit(stmt::Return *stmt)
{
    Value value = std::nullopt;
    if (stmt->m_value.has_value()) value = evaluate(stmt->m_value->get());

    // using exceptions for returning from functions is fucking horrible,
    // but I couldn't come up with anything else
    throw Return{value};
}

void Interpreter::execute_block(const std::vector<StatementPtr> &statements, const std::shared_ptr<Environment> &env)
{
    std::shared_ptr<Environment> previous = m_env;

    // I have no idea why, but without
    // the try - catch block it just does not work
    try
    {
        m_env = env;

        for (auto &statement : statements)
        {
            execute(statement.get());
        }
    }
    catch (...)
    {
        m_env = previous;
        throw;
    }

    m_env = previous;
}

void Interpreter::resolve(expr::Expression *expr, int depth)
{
    m_locals.emplace(expr, depth);
}

Value Interpreter::evaluate(expr::Expression *expr)
{
    return expr->accept(this);
}

void Interpreter::execute(stmt::Statement *stmt)
{
    // skip statements with errors
    if (stmt == nullptr) return;

    stmt->accept(this);
}

Value Interpreter::lookup_variable(const Token &name, expr::Expression *expr)
{
    auto distance = m_locals.find(expr);

    Value val = std::nullopt;
    if (distance != m_locals.end())
        val = m_env->get_at(distance->second, name.get_lexeme());
    else
        val = m_globals->get(name);

    check_null(val, name);
    return val;
}

void Interpreter::check_null(const Value &value, const Token &name)
{
    // report a runtime error if the variable is uninitialized
    if (!value.m_value.has_value()) throw RuntimeError{name, "Variable '" + name.get_lexeme() + "' is uninitialized."};
}

void Interpreter::register_native_funcs()
{
    m_globals = std::make_shared<Environment>();
    m_env = m_globals;

    // clock()
    auto clock = std::make_shared<ClockFunction>();
    m_globals->define("clock", std::dynamic_pointer_cast<Callable>(clock));

    // println()
    auto println = std::make_shared<PrintlnFunction>();
    m_globals->define("println", std::dynamic_pointer_cast<Callable>(println));
}

bool Interpreter::is_true(const Value &val)
{
    // only `nil` and `false` are false in cpplox
    if (val.m_value == std::nullopt) return false;
    if (std::holds_alternative<bool>(val.m_value.value())) return std::get<bool>(val.m_value.value());

    return true;
}

bool Interpreter::is_equal(const Value &val1, const Value &val2)
{
    // if both values are `nil` they are equal
    // useful when checking if the value is null
    if (val1.m_value == std::nullopt && val2.m_value == std::nullopt) return true;
    if (val1.m_value == std::nullopt) return false;

    return val1.m_value.value() == val2.m_value.value();
}

void Interpreter::check_number_operands(const Token &op, const Value &operand)
{
    if (std::holds_alternative<double>(operand.m_value.value())) return;

    throw RuntimeError{op, "Operand must be a number."};
}

void Interpreter::check_number_operands(const Token &op, const Value &left, const Value &right)
{
    if (std::holds_alternative<double>(left.m_value.value()) && std::holds_alternative<double>(right.m_value.value()))
        return;

    throw RuntimeError{op, "Operands must be numbers."};
}

}
