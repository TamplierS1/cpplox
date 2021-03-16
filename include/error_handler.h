#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <iostream>

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

    void error(unsigned int line, const std::string& msg);
    void debug_error(const std::string& msg);

    bool m_had_error = false;

private:
    ErrorHandler() = default;

    void report(unsigned int line, const std::string& where, const std::string& msg);
};
}  // namespace garm

#endif  // ERROR_HANDLER_H