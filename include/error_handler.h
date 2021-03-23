#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <exception>
#include <iostream>

#include "token.h"

namespace garm
{
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

    void error(const types::Token& token, const std::string& msg);
    void error(unsigned int line, const std::string& msg);

    void debug_error(const std::string& msg);

    bool m_had_error = false;

private:
    ErrorHandler() = default;

    void report(unsigned int line, const std::string& where, const std::string& msg);
};

class ParseError : public std::exception
{
public:
    [[nodiscard]] const char* what() const noexcept override;
};
}

#endif  // ERROR_HANDLER_H