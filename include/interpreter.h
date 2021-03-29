#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>

#include "environment.h"
#include "error_handler.h"
#include "syntax_tree/expression.h"
#include "syntax_tree/statement.h"

namespace cpplox
{
using namespace ast;

// Interprets a syntax tree and executes it
class Interpreter : public expr::Visitor, stmt::Visitor
{
public:
    Interpreter()
    {
        m_env = std::make_shared<Environment>();
    }

    void interpret(std::vector<StatementPtr>& stmts);

    // expressions
    Value visit(expr::Literal* expr) override;
    Value visit(expr::Grouping* expr) override;
    Value visit(expr::Unary* expr) override;
    Value visit(expr::Binary* expr) override;
    Value visit(expr::Variable* expr) override;
    Value visit(expr::Assign* expr) override;
    Value visit(expr::Logical* expr) override;

    // statements
    void visit(stmt::Expression* stmt) override;
    void visit(stmt::Print* stmt) override;
    void visit(stmt::Var* stmt) override;
    void visit(stmt::Block* stmt) override;
    void visit(stmt::If* stmt) override;
    void visit(stmt::While* stmt) override;

private:
    Value evaluate(expr::Expression* expr);
    void execute(stmt::Statement* stmt);
    void execute_block(const std::vector<StatementPtr>& statements, const std::shared_ptr<Environment>& env);

    bool is_true(const Value& val);
    bool is_equal(const Value& val1, const Value& val2);

    void check_number_operands(const Token& op, const Value& operand);
    void check_number_operands(const Token& op, const Value& left, const Value& right);

    std::shared_ptr<Environment> m_env;
};

}

#endif  // INTERPRETER_H