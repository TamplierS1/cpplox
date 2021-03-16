#include "scanner.h"

namespace garm
{
void Scanner::run_file(const std::string& filename)
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

        if (ErrorHandler::get_instance().m_had_error) std::exit(65);
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "Error: failed to open script file - " << e.what() << '\n';
        return;
    }
}

void Scanner::run_prompt()
{
    std::string line;
    while (std::cout << "> " && std::getline(std::cin, line))
    {
        m_start = 0;
        m_current = 0;
        m_source = line;

        // scan the line
        run();

        std::cout << '\n';

        ErrorHandler::get_instance().m_had_error = false;
    }
}

void Scanner::run()
{
    std::vector<Token> tokens = scan_tokens();
    for (const Token& token : tokens)
    {
        std::cout << token.get_lexeme();
    }
}

std::vector<Token> Scanner::scan_tokens()
{
    while (!is_end())
    {
        m_start = m_current;
        scan_token();
    }

    m_tokens.emplace_back(TokenType::GARM_EOF, "", std::nullopt, m_line);
    return m_tokens;
}

void Scanner::scan_token()
{
    // get the next char
    char c = advance();
    switch (c)
    {
        case '(':
            add_token(TokenType::LEFT_PAREN, std::nullopt);
            break;
        case ')':
            add_token(TokenType::RIGHT_PAREN, std::nullopt);
            break;
        case '{':
            add_token(TokenType::LEFT_BRACE, std::nullopt);
            break;
        case '}':
            add_token(TokenType::RIGHT_BRACE, std::nullopt);
            break;
        case ',':
            add_token(TokenType::COMMA, std::nullopt);
            break;
        case '.':
            add_token(TokenType::DOT, std::nullopt);
            break;
        case '-':
            add_token(TokenType::MINUS, std::nullopt);
            break;
        case '+':
            add_token(TokenType::PLUS, std::nullopt);
            break;
        case ';':
            add_token(TokenType::SEMICOLON, std::nullopt);
            break;
        case '*':
            add_token(TokenType::STAR, std::nullopt);
            break;
        case '!':
            // we handle operators like !=, == this way
            add_token(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG, std::nullopt);
            break;
        case '=':
            add_token(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL, std::nullopt);
            break;
        case '<':
            add_token(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS, std::nullopt);
            break;
        case '>':
            add_token(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER, std::nullopt);
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
                add_token(TokenType::SLASH, std::nullopt);
            break;
        case ' ':
        case '\r':
        case '\t':
            break;
        case '\n':
            m_line++;
            break;
        case '"':
            // all strings start with "
            string();
            break;
        default:
            // we encountered a number literal
            if (std::isdigit(c)) number();
            // we encountered an identifier
            else if (std::isalpha(c))
                identifier();
            else
                ErrorHandler::get_instance().error(m_line, "Unexpected character.");
            break;
    }
}

void Scanner::string()
{
    // advance the input until you encounter the closing quote or the end of file
    while (peek() != '"' && !is_end())
    {
        if (peek() == '\n') m_line++;
        advance();
    }

    if (is_end())
    {
        ErrorHandler::get_instance().error(m_line, "Unterminated string.");
    }

    // eat the closing quote
    advance();

    // get the lexeme
    std::string value = m_source.substr(m_start + 1, m_current - m_start - 1);
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
    add_token(TokenType::NUMBER, value);
}

void Scanner::identifier()
{
    while (std::isalnum(peek()))
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
    while (peek() != '*' && !is_end())
    {
        if (peek() == '\n') m_line++;
        // handle block comments
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

void Scanner::add_token(TokenType type, const OptionalLiteral& literal)
{
    std::string lexeme = m_source.substr(m_start, m_current - m_start);
    m_tokens.emplace_back(type, lexeme, literal, m_line);
}

bool Scanner::is_end() const
{
    return m_current >= m_source.size();
}

bool Scanner::match(char expected)
{
    if (is_end()) return false;
    if (m_source.at(m_current) != expected) return false;

    m_current++;
    return true;
}

char Scanner::advance()
{
    return m_source.at(m_current++);
}

char Scanner::peek()
{
    if (is_end()) return '\0';
    return m_source.at(m_current);
}

char Scanner::peek_next()
{
    if (m_current + 1 >= m_source.size()) return '\0';
    return m_source.at(m_current + 1);
}

std::optional<TokenType> Scanner::str_to_keyword(const std::string& str)
{
    static std::map<std::string, TokenType> keyword_lookup{
        {"and", TokenType::AND},     {"class", TokenType::CLASS},   {"else", TokenType::ELSE},
        {"false", TokenType::FALSE}, {"for", TokenType::FOR},       {"fun", TokenType::FUN},
        {"if", TokenType::IF},       {"nil", TokenType::NIL},       {"or", TokenType::OR},
        {"print", TokenType::PRINT}, {"return", TokenType::RETURN}, {"super", TokenType::SUPER},
        {"this", TokenType::THIS},   {"true", TokenType::TRUE},     {"var", TokenType::VAR},
        {"while", TokenType::WHILE}};

    if (keyword_lookup.contains(str))
    {
        return keyword_lookup.find(str)->second;
    }
    else
    {
        return std::nullopt;
    }
}

}  // namespace garm
