#ifndef RESOLVER_H
#define RESOLVER_H

#include <deque>
#include <unordered_map>

#include "error_handler.h"
#include "interpreter.h"
#include "syntax_tree/expression.h"
#include "syntax_tree/statement.h"

namespace cpplox
{
using namespace ast;

// strings are variable names
// bools indicate whether we have finished resolving the variable's initializer
using Scope = std::unordered_map<std::string, bool>;

// Resolves variable bindings (except global variables)
class Resolver : public stmt::Visitor, expr::Visitor
{
    enum class FunctionType
    {
        NONE,
        FUNCTION
    };

public:
    explicit Resolver(const std::shared_ptr<Interpreter>& interpreter)
        : m_interpreter(interpreter)
    {
    }

    void visit(stmt::Block* stmt) override;
    void visit(stmt::Var* stmt) override;
    void visit(stmt::Function* stmt) override;
    void visit(stmt::Expression* stmt) override;
    void visit(stmt::If* stmt) override;
    void visit(stmt::Print* stmt) override;
    void visit(stmt::Return* stmt) override;
    void visit(stmt::While* stmt) override;

    Value visit(expr::Variable* expr) override;
    Value visit(expr::Assign* expr) override;
    Value visit(expr::Lambda* expr) override;
    Value visit(expr::Binary* expr) override;
    Value visit(expr::Call* expr) override;
    Value visit(expr::Grouping* expr) override;
    Value visit(expr::Literal* expr) override;
    Value visit(expr::Logical* expr) override;
    Value visit(expr::Unary* expr) override;

    void resolve(const std::vector<StatementPtr>& stmts);

private:
    void resolve(stmt::Statement* stmt);
    void resolve(expr::Expression* expr);
    void resolve_local(expr::Expression* expr, const Token& name);
    void resolve_function(stmt::Function* function, FunctionType type);

    void begin_scope();
    void end_scope();

    void declare(const Token& name);
    void define(const Token& name);

    // scopes that are currently in scope
    std::deque<Scope> m_scopes;
    std::weak_ptr<Interpreter> m_interpreter;

    FunctionType m_current_func = FunctionType::NONE;
};

}

#endif  // RESOLVER_H