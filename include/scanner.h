#ifndef SCANNER_H
#define SCANNER_H

#include <cctype>
#include <fstream>
#include <sstream>
#include <vector>

#include "error_handler.h"
#include "token.h"
#include "value.h"

namespace garm
{
using namespace garm::types;

class Scanner
{
public:
    // scans file
    std::optional<std::vector<Token>> run_file(const std::string& filename);
    // starts up an interactive prompt
    void run_prompt();

private:
    // scans the source string and returns a vector of tokens
    std::vector<Token> run();
    // scan a string of characters and turn them into tokens
    std::vector<Token> scan_tokens();
    // scan an individual character and turn it into a token
    void scan_token();

    /* Different methods for scanning longer lexemes */
    void string();
    void number();
    void identifier();
    void block_comment();

    void add_token(TokenType type, const Value& literal);
    // returns true if scanner is at the end of file
    [[nodiscard]] bool is_end() const;
    // returns true if the next char in the input equals *expected*
    bool match(char expected);
    // advances the input further and returns the next char
    char advance();
    // returns the next char in input (1-char lookahead)
    char peek();
    // returns the char after the next char (2-char lookahead)
    char peek_next();

    static std::optional<TokenType> str_to_keyword(const std::string& str);

    std::vector<Token> m_tokens;

    // start of the current token
    unsigned int m_start = 0;
    // current index into the source string
    unsigned int m_current = 0;
    // current line
    unsigned int m_line = 1;

    std::string m_source;
};
}  // namespace garm

#endif  // SCANNER_H