#include "scanner.h"

namespace cpplox
{
std::optional<std::vector<Token>> Scanner::run_file(const std::string& filename)
{
    try
    {
        std::ifstream file_stream{filename};
        file_stream.exceptions(std::ifstream::badbit | std::ifstream::failbit);

        std::ostringstream sstream;
        sstream << file_stream.rdbuf();

        m_source = sstream.str();

        // scan the m_source that contains the script file contents
        run();

        if (ReportError::g_had_error)
            std::exit(65);
        if (ReportError::g_had_runtime_error)
            std::exit(70);

        return m_tokens;
    }
    catch (std::ifstream::failure& e)
    {
        ReportError::error(1, 1, ' ', "", "Failed to open script file.");
        return std::nullopt;
    }
}

std::vector<Token> Scanner::run_line(const std::string& line)
{
    m_start = 0;
    m_current = 0;
    m_source = line;
    m_tokens.clear();

    ReportError::g_had_error = false;
    ReportError::g_had_runtime_error = false;

    run();

    return m_tokens;
}

void Scanner::run()
{
    scan_tokens();
}

std::vector<Token> Scanner::scan_tokens()
{
    update_source_line();
    while (!is_end())
    {
        m_start = m_current;
        scan_token();
    }

    m_tokens.emplace_back(TokenType::cpplox_EOF, "", std::nullopt, m_line, "", m_column);
    return m_tokens;
}

void Scanner::scan_token()
{
    // get the next char
    char c = advance();
    switch (c)
    {
        case '(':
            add_token(TokenType::LEFT_PAREN);
            break;
        case ')':
            add_token(TokenType::RIGHT_PAREN);
            break;
        case '{':
            add_token(TokenType::LEFT_BRACE);
            break;
        case '}':
            add_token(TokenType::RIGHT_BRACE);
            break;
        case ',':
            add_token(TokenType::COMMA);
            break;
        case '.':
            add_token(TokenType::DOT);
            break;
        case '-':
            add_token(TokenType::MINUS);
            break;
        case '+':
            add_token(TokenType::PLUS);
            break;
        case ';':
            add_token(TokenType::SEMICOLON);
            break;
        case '*':
            add_token(TokenType::STAR);
            break;
        case '!':
            // handle operators like !=, ==
            add_token(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
            break;
        case '=':
            add_token(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
            break;
        case '<':
            add_token(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
            break;
        case '>':
            add_token(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
            break;
        case '/':
            // if it's a comment - skip
            if (match('/'))
            {
                while (peek() != '\n' && !is_end())
                    advance();
            }
            else if (match('*'))  // block comment
            {
                block_comment();
            }
            else
                add_token(TokenType::SLASH);
            break;
        case ' ':
        case '\r':
        case '\t':
            break;
        case '\n':
            update_source_line();
            m_line++;
            break;
        case '"':
            // all strings start with "
            string();
            break;
        default:
            // we encountered a number literal
            if (std::isdigit(c))
                number();
            // we encountered an identifier
            else if (std::isalpha(c))
                identifier();
            else
                ReportError::error(m_line, m_column, c, m_str_line, "Unexpected character.");
            break;
    }
}

void Scanner::string()
{
    // advance the input until you encounter the closing quote or the end of file
    while (peek() != '"' && !is_end())
    {
        if (peek() == '\n')
            m_line++;
        advance();
    }

    if (is_end())
    {
        ReportError::error(m_line, m_column, m_source[m_current], m_str_line, "Unterminated string.");
    }

    // eat the closing quote
    advance();

    // get the lexeme
    std::string value = m_source.substr(m_start + 1, m_current - m_start - 2);
    add_token(TokenType::STRING, value);
}

void Scanner::number()
{
    while (std::isdigit(peek()))
        advance();

    // handle decimal numbers
    if (peek() == '.' && std::isdigit(peek_next()))
    {
        advance();

        while (std::isdigit(peek()))
            advance();
    }

    double value = std::stod(m_source.substr(m_start, m_current - m_start));
    add_token(TokenType::NUMBER, static_cast<Value>(value));
}

void Scanner::identifier()
{
    while (std::isalnum(peek()) || peek() == '_')
        advance();

    std::string lexeme = m_source.substr(m_start, m_current - m_start);

    TokenType type;
    std::optional<TokenType> type_opt = str_to_keyword(lexeme);
    // the identifier you encountered is a keyword
    if (type_opt.has_value())
        type = type_opt.value();
    else  // or it's just an identifier
        type = TokenType::IDENTIFIER;

    // it doesn't have any literal value - so just std::nullopt
    add_token(type, std::nullopt);
}

void Scanner::block_comment()
{
    while (peek() != '*')
    {
        // if we run out of characters
        if (is_end())
        {
            ReportError::error(m_line, m_column, m_source[m_current], m_str_line, "Unclosed block comment.");
            return;
        }
        if (peek() == '\n')
            m_line++;
        // handle nested comments
        if (peek_next() == '*')
            if (peek() == '/')
            {
                // eat '/*'
                advance();
                advance();

                block_comment();
            }

        advance();
    }

    // eat '*/'
    advance();
    advance();
}

void Scanner::add_token(TokenType type, const Value& literal)
{
    std::string lexeme = m_source.substr(m_start, m_current - m_start);
    m_tokens.emplace_back(type, lexeme, literal, m_line, m_str_line, m_column);
}

bool Scanner::is_end() const
{
    return m_current >= m_source.size();
}

bool Scanner::match(char expected)
{
    if (is_end())
        return false;
    if (m_source.at(m_current) != expected)
        return false;

    m_current++;
    return true;
}

char Scanner::advance()
{
    update_column();
    return m_source.at(m_current++);
}

char Scanner::peek()
{
    if (is_end())
        return '\0';
    return m_source.at(m_current);
}

char Scanner::peek_next()
{
    if (m_current + 1 >= m_source.size())
        return '\0';
    return m_source.at(m_current + 1);
}

void Scanner::update_source_line()
{
    // the character that starts new line
    int new_line = m_current;
    for (char c : m_source.substr(m_current, m_source.size() - m_current))
    {
        if (c == '\n')
            break;
        new_line++;
    }

    m_str_line = m_source.substr(m_current, new_line - m_current);
    m_new_line = new_line;
}

void Scanner::update_column()
{
    if (m_source[m_current] == '\n')
        m_column = 0;
    else
        m_column++;
}

std::optional<TokenType> Scanner::str_to_keyword(const std::string& str)
{
    static std::map<std::string, TokenType> keyword_lookup{
        {"and", TokenType::AND},     {"class", TokenType::CLASS},   {"else", TokenType::ELSE},
        {"false", TokenType::FALSE}, {"for", TokenType::FOR},       {"fun", TokenType::FUN},
        {"if", TokenType::IF},       {"nil", TokenType::NIL},       {"or", TokenType::OR},
        {"print", TokenType::PRINT}, {"return", TokenType::RETURN}, {"super", TokenType::SUPER},
        {"this", TokenType::THIS},   {"true", TokenType::TRUE},     {"var", TokenType::VAR},
        {"while", TokenType::WHILE}, {"static", TokenType::PREFIX}, {"import", TokenType::IMPORT}};

    if (keyword_lookup.contains(str))
    {
        return keyword_lookup.find(str)->second;
    }
    else
    {
        return std::nullopt;
    }
}

}
