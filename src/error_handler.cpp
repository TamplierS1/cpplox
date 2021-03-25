#include "error_handler.h"

namespace garm
{

const char* ParseError::what() const noexcept
{
    return "ParseError: unexpected character.";
}

const char* RuntimeError::what() const noexcept
{
    return m_msg.c_str();
}

void ErrorHandler::error(const types::Token& token, const std::string& msg)
{
    if (token.get_token_type() == types::TokenType::GARM_EOF)
        report(token.get_line(), " at end", msg);
    else
        report(token.get_line(), " at '" + token.get_lexeme() + "'", msg);
}

void ErrorHandler::error(unsigned int line, const std::string& msg)
{
    report(line, "", msg);
}

void ErrorHandler::runtime_error(const RuntimeError& error)
{
    std::cout << error.m_msg << "\n[line " + std::to_string(error.m_op.get_line()) << "]";
    m_had_runtime_error = true;
}

void ErrorHandler::debug_error(const std::string& msg)
{
    std::cout << "Internal error at " << __FILE__ << " line" << __LINE__ << ": " << msg << '\n';
}

void ErrorHandler::report(unsigned int line, const std::string& where, const std::string& msg)
{
    std::cout << "[line " << line << "]"
              << " Error" << where << ": " << msg << '\n';
    m_had_error = true;
}

}
