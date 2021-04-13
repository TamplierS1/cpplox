#include "error_handler.h"

namespace cpplox
{
const char* ParseError::what() const noexcept
{
    return "ParseError: unexpected character.";
}

const char* RuntimeError::what() const noexcept
{
    return m_msg.c_str();
}

void ErrorHandler::error(const Token& token, const std::string& msg)
{
    std::string formatted_line = format_msg(token, fmt::color::red);

    if (token.token_type() == TokenType::cpplox_EOF)
        report_error(token.line(), token.column(), "at end", formatted_line, msg);
    else
        report_error(token.line(), token.column(), token.lexeme(), formatted_line, msg);
}

void ErrorHandler::error(int line, int column, char character, const std::string& src_str, const std::string& msg)
{
    report_error(line, column, std::string(1, character), src_str, msg);
}

void ErrorHandler::runtime_error(const RuntimeError& error)
{
    this->error(error.m_op, error.m_msg);
    m_had_runtime_error = true;
}

void ErrorHandler::debug_error(const std::string& msg, int line)
{
    fmt::print("\n[line {}]", line);
    fmt::print(fmt::emphasis::italic | fg(fmt::color::red), " Internal error in ");
    fmt::print(fmt::emphasis::bold | fg(fmt::color::deep_pink), "{}:\n\n", __FILE__);
    fmt::print(fmt::emphasis::bold | fg(fmt::color::white), "\t{}\n\n", msg);
}

void ErrorHandler::warning(const std::string& msg) const
{
    fmt::print(fmt::emphasis::italic | fg(fmt::color::yellow), " Warning: {}\n\n", msg);
}

void ErrorHandler::warning(const Token& token, const std::string& msg) const
{
    std::string formatted_line = format_msg(token, fmt::color::yellow);
    report_warning(token.line(), token.column(), token.lexeme(), formatted_line, msg);
}

std::string ErrorHandler::format_msg(const Token& token, fmt::color token_color) const
{
    std::string source_line = token.str_line();

    // I separated the line into multiple variables
    // because I wanted the token to be colored differently from the rest of the line

    // part of the line before the token that caused the error
    int column = token.column();
    std::string token_str = fmt::format(fg(token_color), "{}", token.lexeme());
    std::string before_token_line;
    std::string after_token_line;
    // Handle tokens like `>=` that are 2 characters wide
    if (token.lexeme().size() == 2)
    {
        before_token_line = fmt::format(fg(fmt::color::dark_olive_green), "{}",
                                        source_line.substr(0, column - token.lexeme().size() + 1));
        after_token_line = fmt::format(fg(fmt::color::dark_olive_green), "{}",
                                       source_line.substr(column + 1, source_line.size() - column));
    }
    else
    {
        before_token_line =
            fmt::format(fg(fmt::color::dark_olive_green), "{}", source_line.substr(0, column - token.lexeme().size()));
        after_token_line = fmt::format(fg(fmt::color::dark_olive_green), "{}",
                                       source_line.substr(column, source_line.size() - column));
    }

    return before_token_line + token_str + after_token_line;
}

void ErrorHandler::report_error(int line, int column, const std::string& where, const std::string& src_str,
                                const std::string& msg)
{
    fmt::print("\n[{}, {}]", line, column);
    fmt::print(fmt::emphasis::italic | fg(fmt::color::red), " Error at '");
    fmt::print(fmt::emphasis::bold | fg(fmt::color::white), "{}", where);
    fmt::print(fmt::emphasis::italic | fg(fmt::color::red), "': {}\n\n", msg);
    fmt::print(fg(fmt::color::dark_olive_green), "\t{}\n\n", src_str);
    m_had_error = true;
}

void ErrorHandler::report_warning(int line, int column, const std::string& where, const std::string& src_str,
                                  const std::string& msg) const
{
    fmt::print("\n[{}, {}]", line, column);
    fmt::print(fmt::emphasis::italic | fg(fmt::color::yellow), " Warning at '");
    fmt::print(fmt::emphasis::bold | fg(fmt::color::white), "{}", where);
    fmt::print(fmt::emphasis::italic | fg(fmt::color::yellow), "': {}\n\n", msg);
    fmt::print(fg(fmt::color::dark_olive_green), "\t{}\n\n", src_str);
}

}
