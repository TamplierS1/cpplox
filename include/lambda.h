#ifndef LAMBDA_H
#define LAMBDA_H

#include "callable.h"
#include "syntax_tree/expression.h"
#include "environment.h"

namespace cpplox
{
class Lambda : public Callable
{
public:
    explicit Lambda(const std::shared_ptr<ast::expr::Lambda>& declaration)
        : m_declaration(declaration)
    {
    }

    Value call(Interpreter* interpreter, const std::vector<Value>& args) override;
    [[nodiscard]] unsigned int arity() const override;
    [[nodiscard]] std::string to_string() const;

private:
    std::shared_ptr<ast::expr::Lambda> m_declaration;
};

}

#endif  // LAMBDA_H