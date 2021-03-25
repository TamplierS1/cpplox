#include "interpreter.h"

namespace garm
{

void Interpreter::interpret(ast::Expression* expr)
{
    try
    {
        std::cout << evaluate(expr);
    }
    catch (RuntimeError& e)
    {
        ErrorHandler::get_instance().runtime_error(e);
    }
}

Value Interpreter::visit(ast::Literal *expr)
{
    // just return the literal value, that we put there earlier in the scanner
    return *expr->m_value;
}

Value Interpreter::visit(ast::Grouping *expr)
{
    // to evaluate parenthesis just evaluate the expression inside them
    return evaluate(expr->m_expression.get());
}

Value Interpreter::visit(ast::Unary *expr)
{
    Value right = evaluate(expr->m_right.get());

    switch (expr->m_op->get_token_type())
    {
        case types::TokenType::MINUS:
            check_number_operands(*expr->m_op, right);
            // a unary minus can only be applied to numbers and that's why we convert the expression to double
            return -std::get<double>(right.value());
        case types::TokenType::BANG:
            return !is_true(right);
    }

    return std::nullopt;
}

Value Interpreter::visit(ast::Binary *expr)
{
    Value left = evaluate(expr->m_left.get());
    Value right = evaluate(expr->m_right.get());

    // extract the values beforehand so as not to repeat it
    double dleft = 0, dright = 0;
    std::string strleft, strright;
    bool is_numbers = false, is_strings = false;
    // numbers
    if (std::holds_alternative<double>(left.value()) && std::holds_alternative<double>(right.value()))
    {
        dleft = std::get<double>(left.value());
        dright = std::get<double>(right.value());
        is_numbers = true;
    }
    // strings
    else if (std::holds_alternative<std::string>(left.value()) && std::holds_alternative<std::string>(right.value()))
    {
        strleft = std::get<std::string>(left.value());
        strright = std::get<std::string>(right.value());
        is_strings = true;
    }

    switch (expr->m_op->get_token_type())
    {
            /* Arithmetic */
        case types::TokenType::MINUS:
            check_number_operands(*expr->m_op, left, right);
            return dleft - dright;
        case types::TokenType::PLUS:
            if (is_numbers) return dleft + dright;
            if (is_strings) return strleft + strright;

            throw RuntimeError{*expr->m_op, "Operands must be two numbers or two strings."};
        case types::TokenType::SLASH:
            check_number_operands(*expr->m_op, left, right);
            return dleft / dright;
        case types::TokenType::STAR:
            check_number_operands(*expr->m_op, left, right);
            return dleft * dright;
            /* Comparison */
        case types::TokenType::GREATER:
            check_number_operands(*expr->m_op, left, right);
            return dleft > dright;
        case types::TokenType::GREATER_EQUAL:
            check_number_operands(*expr->m_op, left, right);
            return dleft >= dright;
        case types::TokenType::LESS:
            check_number_operands(*expr->m_op, left, right);
            return dleft < dright;
        case types::TokenType::LESS_EQUAL:
            check_number_operands(*expr->m_op, left, right);
            return dleft <= dright;
        case types::TokenType::BANG_EQUAL:
            return !is_equal(left, right);
        case types::TokenType::EQUAL_EQUAL:
            return is_equal(left, right);
    }

    return std::nullopt;
}

Value Interpreter::evaluate(ast::Expression *expr)
{
    return expr->accept(this);
}

bool Interpreter::is_true(const Value &val)
{
    // only `nil` and `false` are false in garm
    if (val == std::nullopt) return false;
    if (std::holds_alternative<bool>(val.value())) return std::get<bool>(val.value());

    return true;
}

bool Interpreter::is_equal(const Value &val1, const Value& val2)
{
    // if both values are `nil` they are equal
    // useful when checking if the value is null
    if (val1 == std::nullopt && val2 == std::nullopt) return true;
    if (val1 == std::nullopt) return false;

    return val1.value() == val2.value();
}

void Interpreter::check_number_operands(const types::Token &op, const Value &operand)
{
    if (std::holds_alternative<double>(operand.value())) return;

    throw RuntimeError{op, "Operand must be a number."};
}

void Interpreter::check_number_operands(const types::Token &op, const Value &left, const Value &right)
{
    if (std::holds_alternative<double>(left.value()) && std::holds_alternative<double>(right.value()))
        return;

    throw RuntimeError{op, "Operands must be numbers."};
}

}
