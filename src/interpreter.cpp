#include "interpreter.h"

namespace cpplox
{
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
    // just return the literal value, that we put there earlier in the scanner
    return *expr->m_value;
}

Value Interpreter::visit(expr::Grouping *expr)
{
    // to evaluate parenthesis just evaluate the expression inside them
    return evaluate(expr->m_expression.get());
}

Value Interpreter::visit(expr::Unary *expr)
{
    Value right = evaluate(expr->m_right.get());

    switch (expr->m_op->get_token_type())
    {
        case TokenType::MINUS:
            check_number_operands(*expr->m_op, right);
            // a unary minus can only be applied to numbers and that's why we convert the expression to double
            return -std::get<double>(right.value());
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
    if (std::holds_alternative<double>(left.value()) && std::holds_alternative<double>(right.value()))
    {
        dleft = std::get<double>(left.value());
        dright = std::get<double>(right.value());
        is_numbers = true;
    }

    switch (expr->m_op->get_token_type())
    {
            /* Arithmetic */
        case TokenType::MINUS:
            check_number_operands(*expr->m_op, left, right);
            return dleft - dright;
        case TokenType::PLUS:
            if (is_numbers) return dleft + dright;

            if (std::holds_alternative<std::string>(left.value()) && std::holds_alternative<std::string>(right.value()))
            {
                return std::get<std::string>(left.value()) + std::get<std::string>(right.value());
            }

            // if one of the values is a string,
            // the other one is converted to a string and concatenated
            if (std::holds_alternative<std::string>(left.value()))
            {
                return std::get<std::string>(left.value()) +
                       trim_zeroes(std::to_string(std::get<double>(right.value())));
            }
            else if (std::holds_alternative<std::string>(right.value()))
            {
                return std::get<std::string>(right.value()) +
                       trim_zeroes(std::to_string(std::get<double>(left.value())));
            }
        case TokenType::SLASH:
            check_number_operands(*expr->m_op, left, right);
            if (dright == 0) throw RuntimeError{*expr->m_op, "Cannot divide by zero."};
            return dleft / dright;
        case TokenType::STAR:
            check_number_operands(*expr->m_op, left, right);
            return dleft * dright;
            /* Comparison */
        case TokenType::GREATER:
            check_number_operands(*expr->m_op, left, right);
            return dleft > dright;
        case TokenType::GREATER_EQUAL:
            check_number_operands(*expr->m_op, left, right);
            return dleft >= dright;
        case TokenType::LESS:
            check_number_operands(*expr->m_op, left, right);
            return dleft < dright;
        case TokenType::LESS_EQUAL:
            check_number_operands(*expr->m_op, left, right);
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
    Value var = m_env->get(*expr->m_name);
    // report a runtime error if the variable is uninitialized
    if(!var.has_value())
        throw RuntimeError{*expr->m_name, "Variable '" + expr->m_name->get_lexeme() + "' is uninitialized."};

    return var;
}

Value Interpreter::visit(expr::Assign *expr)
{
    Value val = evaluate(expr->m_value.get());
    m_env->assign(*expr->m_name, val);
    return val;
}

void Interpreter::visit(stmt::Expression *stmt)
{
    evaluate(stmt->m_expr.get());
}

void Interpreter::visit(stmt::Print *stmt)
{
    Value value = evaluate(stmt->m_expr.get());
    std::cout << value;
}

void Interpreter::visit(stmt::Var *stmt)
{
    Value value = std::nullopt;
    if (stmt->m_initializer.has_value())
    {
        value = evaluate(stmt->m_initializer->get());
    }

    m_env->define(stmt->m_name->get_lexeme(), value);
}

void Interpreter::visit(stmt::Block *stmt)
{
    execute_block(stmt->m_statements, std::make_shared<Environment>(m_env));
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

void Interpreter::execute_block(const std::vector<StatementPtr> &statements, const std::shared_ptr<Environment> &env)
{
    std::shared_ptr<Environment> previous = m_env;

    m_env = env;

    for (auto &statement : statements)
    {
        execute(statement.get());
    }

    m_env = previous;
}

bool Interpreter::is_true(const Value &val)
{
    // only `nil` and `false` are false in cpplox
    if (val == std::nullopt) return false;
    if (std::holds_alternative<bool>(val.value())) return std::get<bool>(val.value());

    return true;
}

bool Interpreter::is_equal(const Value &val1, const Value &val2)
{
    // if both values are `nil` they are equal
    // useful when checking if the value is null
    if (val1 == std::nullopt && val2 == std::nullopt) return true;
    if (val1 == std::nullopt) return false;

    return val1.value() == val2.value();
}

void Interpreter::check_number_operands(const Token &op, const Value &operand)
{
    if (std::holds_alternative<double>(operand.value())) return;

    throw RuntimeError{op, "Operand must be a number."};
}

void Interpreter::check_number_operands(const Token &op, const Value &left, const Value &right)
{
    if (std::holds_alternative<double>(left.value()) && std::holds_alternative<double>(right.value())) return;

    throw RuntimeError{op, "Operands must be numbers."};
}

}
