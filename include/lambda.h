#ifndef LAMBDA_H
#define LAMBDA_H

#include "callable.h"
#include "environment.h"
#include "syntax_tree/expression.h"

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
    [[nodiscard]] inline int arity() const override
    {
        return m_declaration->m_params.size();
    }
    [[nodiscard]] inline std::string to_string() const override
    {
        return "<fn lambda>";
    }

private:
    std::shared_ptr<ast::expr::Lambda> m_declaration;
};

}

#endif  // LAMBDA_H