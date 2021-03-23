#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <vector>

#include "error_handler.h"
#include "syntax_tree/binary.h"
#include "syntax_tree/expression.h"
#include "syntax_tree/grouping.h"
#include "syntax_tree/literal.h"
#include "syntax_tree/unary.h"
#include "token.h"

namespace garm
{
// Parser that uses recursive decent parsing method
class Parser
{
public:
    explicit Parser(std::vector<types::Token> tokens)
        : m_tokens(std::move(tokens))
    {
    }

    // Begins parsing
    std::optional<ExpressionPtr> parse();

private:
    /*
     * Methods that directly translate to the grammar rules
     */

    ExpressionPtr expression();
    ExpressionPtr equality();
    ExpressionPtr comparison();
    ExpressionPtr term();
    ExpressionPtr factor();
    ExpressionPtr unary();
    ExpressionPtr primary();

    /*
     * Utility functions
     */

    // Checks if the current token is of any of the given types. Consumes the token if the type matches
    bool match(const std::vector<types::TokenType>&& types);
    // Returns the current token and consumes it.
    types::Token advance();
    // Checks if the current token is of the given type, consumes it and returns it.
    // If the types don't match an error is reported
    types::Token consume(types::TokenType type, const std::string& msg);
    // Checks if current token is of the given type.
    [[nodiscard]] bool check(types::TokenType type) const;
    // Returns the current token
    [[nodiscard]] types::Token peek() const;
    // Checks if the input has exhausted
    [[nodiscard]] bool is_end() const;
    // Returns the most recently consumed token
    [[nodiscard]] types::Token previous() const;
    // Reports an error and returns an exception
    [[nodiscard]] ParseError error(const types::Token& token, const std::string& msg);
    // Advances the input to the statement boundary in the process of error recovery
    void synchronize();

    const std::vector<types::Token> m_tokens;
    unsigned int m_current = 0;
};
}

#endif  // PARSER_H