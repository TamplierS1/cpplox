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
    explicit RuntimeError(Token op, std::string msg)
        : m_op(std::move(op))
        , m_msg(std::move(msg))
    {
    }

    [[nodiscard]] const char* what() const noexcept override;

    cpplox::Token m_op;
    std::string m_msg;
};

class ErrorHandler
{
public:
    ErrorHandler(const ErrorHandler&) = delete;
    void operator=(const ErrorHandler&) = delete;

    static ErrorHandler& get_instance()
    {
        static ErrorHandler instance;
        return instance;
    }

    void error(const Token& token, const std::string& msg);
    void error(unsigned int line, unsigned int column, char character, const std::string& src_str,
               const std::string& msg);
    void runtime_error(const RuntimeError& error);

    void debug_error(const std::string& msg);

    bool m_had_error = false;
    bool m_had_runtime_error = false;

private:
    ErrorHandler() = default;

    std::string format_error(const Token& token);

    void report(unsigned int line, unsigned int column, const std::string& where, const std::string& src_str,
                const std::string& msg);
};

}

#endif  // ERROR_HANDLER_H