#include "error.h"

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

namespace ReportError
{
void error(const Token& token, std::string_view msg)
{
    std::string formatted_line = format_msg(token, fmt::color::red);

    if (token.token_type() == TokenType::cpplox_EOF)
        report_error(token.line(), token.column(), "at end", formatted_line, msg);
    else
        report_error(token.line(), token.column(), token.lexeme(), formatted_line, msg);
}

void error(int line, int column, char character, std::string_view src_str, std::string_view msg)
{
    report_error(line, column, std::string(1, character), src_str, msg);
}

void runtime_error(const RuntimeError& run_error)
{
    error(run_error.m_op, run_error.m_msg);
    g_had_runtime_error = true;
}

void debug_error(std::string_view msg, int line)
{
    fmt::print("\n[line {}]", line);
    fmt::print(fmt::emphasis::italic | fg(fmt::color::red), " Internal error in ");
    fmt::print(fmt::emphasis::bold | fg(fmt::color::deep_pink), "{}:\n\n", __FILE__);
    fmt::print(fmt::emphasis::bold | fg(fmt::color::white), "\t{}\n\n", msg);
}

void warning(std::string_view msg)
{
    fmt::print(fmt::emphasis::italic | fg(fmt::color::yellow), " Warning: {}\n\n", msg);
}

void warning(const Token& token, const std::string& msg)
{
    std::string formatted_line = format_msg(token, fmt::color::yellow);
    report_warning(token.line(), token.column(), token.lexeme(), formatted_line, msg);
}

std::string format_msg(const Token& token, fmt::color token_color)
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

void report_error(int line, int column, std::string_view where, std::string_view src_str, std::string_view msg)
{
    fmt::print("\n[{}, {}]", line, column);
    fmt::print(fmt::emphasis::italic | fg(fmt::color::red), " Error at '");
    fmt::print(fmt::emphasis::bold | fg(fmt::color::white), "{}", where);
    fmt::print(fmt::emphasis::italic | fg(fmt::color::red), "': {}\n\n", msg);
    fmt::print(fg(fmt::color::dark_olive_green), "\t{}\n\n", src_str);
    g_had_error = true;
}

void report_warning(int line, int column, std::string_view where, std::string_view src_str, std::string_view msg)
{
    fmt::print("\n[{}, {}]", line, column);
    fmt::print(fmt::emphasis::italic | fg(fmt::color::yellow), " Warning at '");
    fmt::print(fmt::emphasis::bold | fg(fmt::color::white), "{}", where);
    fmt::print(fmt::emphasis::italic | fg(fmt::color::yellow), "': {}\n\n", msg);
    fmt::print(fg(fmt::color::dark_olive_green), "\t{}\n\n", src_str);
}

}

}
