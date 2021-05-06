#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <exception>
#include <iostream>

#include "fmt/color.h"
#include "fmt/core.h"
#include "token.h"

namespace cpplox
{
class ParseError : public std::exception
{
public:
    [[nodiscard]] const char* what() const noexcept override;
};

class RuntimeError : public std::exception
{
public:
    RuntimeError(Token op, std::string msg)
        : m_op(std::move(op))
        , m_msg(std::move(msg))
    {
    }

    [[nodiscard]] const char* what() const noexcept override;

    Token m_op;
    std::string m_msg;
};

namespace ReportError
{
static bool g_had_error = false;
static bool g_had_runtime_error = false;

void error(const Token& token, std::string_view msg);
void error(int line, int column, char character, std::string_view src_str, std::string_view msg);
void runtime_error(const RuntimeError& run_error);
void debug_error(std::string_view msg, int line);

void warning(std::string_view msg);
void warning(const Token& token, std::string_view msg);

[[nodiscard]] std::string format_msg(const Token& token, fmt::color token_color);

void report_error(int line, int column, std::string_view where, std::string_view src_str, std::string_view msg);
void report_warning(int line, int column, std::string_view where, std::string_view src_str, std::string_view msg);
}

}

#endif  // ERROR_HANDLER_H
