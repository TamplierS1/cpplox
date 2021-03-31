#ifndef FUNCTION_H
#define FUNCTION_H

#include "callable.h"
#include "syntax_tree/statement.h"
#include "environment.h"

namespace cpplox
{
// Represents a user-defined function
class Function : public Callable
{
public:
    explicit Function(const std::shared_ptr<ast::stmt::Function>& declaration,
                      const std::shared_ptr<Environment>& closure)
        :m_declaration(declaration), m_closure(closure)
    {}

    Value call(Interpreter* interpreter, const std::vector<Value>& args) override;
    [[nodiscard]] unsigned int arity() const override;
    [[nodiscard]] std::string to_string() const ;
private:
    std::shared_ptr<ast::stmt::Function> m_declaration;
    std::shared_ptr<Environment> m_closure;
};

}

#endif // FUNCTION_H