#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>
#include <deque>

#include "class.h"
#include "environment.h"
#include "error_handler.h"
#include "fmt/core.h"
#include "function.h"
#include "lambda.h"
#include "native_functions/clock_fn.h"
#include "native_functions/println.h"
#include "return.h"
#include "syntax_tree/expression.h"

namespace cpplox
{
using namespace ast;

// Interprets a syntax tree and executes it
class Interpreter : public expr::Visitor, stmt::Visitor
{
public:
    explicit Interpreter(const std::deque<StatementPtr>& stmts)
        : m_to_interpret(stmts)
    {
        register_native_funcs();
    }

    void interpret();
    void add_statements(const std::vector<StatementPtr>& new_statements);

    // expressions
    Value visit(expr::Literal* expr) override;
    Value visit(expr::Grouping* expr) override;
    Value visit(expr::Unary* expr) override;
    Value visit(expr::Binary* expr) override;
    Value visit(expr::Variable* expr) override;
    Value visit(expr::Assign* expr) override;
    Value visit(expr::Logical* expr) override;
    Value visit(expr::Call* expr) override;
    Value visit(expr::Lambda* expr) override;
    Value visit(expr::Get* expr) override;
    Value visit(expr::Set* expr) override;
    Value visit(expr::This* expr) override;
    Value visit(expr::Super* expr) override;

    // statements
    void visit(stmt::Expression* stmt) override;
    void visit(stmt::Print* stmt) override;
    void visit(stmt::Var* stmt) override;
    void visit(stmt::Block* stmt) override;
    void visit(stmt::If* stmt) override;
    void visit(stmt::While* stmt) override;
    void visit(stmt::Function* stmt) override;
    void visit(stmt::Return* stmt) override;
    void visit(stmt::Class* stmt) override;
    void visit(stmt::Import* stmt) override;

    void execute_block(const std::vector<StatementPtr>& statements, const std::shared_ptr<Environment>& env);
    void resolve(expr::Expression* expr, int depth);

    [[nodiscard]] inline std::shared_ptr<Environment> get_scope() const
    {
        return m_env;
    }

    // global scope bindings
    std::shared_ptr<Environment> m_globals;

private:
    Value evaluate(expr::Expression* expr);
    void execute(stmt::Statement* stmt);

    Value lookup_variable(const Token& name, expr::Expression* expr);
    void check_null(const Value& value, const Token& name);

    void register_native_funcs();

    bool is_true(const Value& val);
    bool is_equal(const Value& val1, const Value& val2);

    void check_number_operands(const Token& op, const Value& operand);
    void check_number_operands(const Token& op, const Value& left, const Value& right);

    // Code to interpret
    std::deque<StatementPtr> m_to_interpret;
    // current scope bindings
    std::shared_ptr<Environment> m_env;
    // resolution information
    // I used raw pointers here, because when looking up a variable,
    // the keys in this map are compared with a raw pointer to an expression
    std::unordered_map<expr::Expression*, int> m_locals;
};

}

#endif  // INTERPRETER_H