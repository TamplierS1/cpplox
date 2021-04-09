#ifndef FUNCTION_H
#define FUNCTION_H

#include "callable.h"
#include "environment.h"
#include "syntax_tree/statement.h"

namespace cpplox
{
// Represents a user-defined function
class Function : public Callable
{
public:
    Function(const std::shared_ptr<ast::stmt::Function>& declaration, const std::shared_ptr<Environment>& closure,
             bool is_initializer = false, bool is_static = false)
        : m_is_static(is_static)
        , m_declaration(declaration)
        , m_closure(closure)
        , m_is_initializer(is_initializer)
    {
    }

    Value call(Interpreter* interpreter, const std::vector<Value>& args) override;

    [[nodiscard]] inline int arity() const override
    {
        return m_declaration->m_params.size();
    }
    [[nodiscard]] inline std::string to_string() const override
    {
        return "<fn " + m_declaration->m_name.lexeme() + ">";
    }
    // Bind `this` field to this function's closure.
    // `this` represents the instance the function has been called on.
    std::shared_ptr<Function> bind(const std::shared_ptr<Instance>& instance);

    // Is this a static method on a class
    const bool m_is_static = false;

private:
    std::shared_ptr<ast::stmt::Function> m_declaration;
    std::shared_ptr<Environment> m_closure;

    const bool m_is_initializer;
};

}

#endif  // FUNCTION_H