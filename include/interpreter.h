#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "syntax_tree/visitor.h"
#include "syntax_tree/literal.h"
#include "syntax_tree/grouping.h"
#include "syntax_tree/binary.h"
#include "syntax_tree/unary.h"
#include "error_handler.h"

namespace garm
{

// Interprets a syntax tree and executes it
class Interpreter : public ast::Visitor
{
public:
    void interpret(ast::Expression* expr);

    Value visit(ast::Literal* expr) override;
    Value visit(ast::Grouping* expr) override;
    Value visit(ast::Unary* expr) override;
    Value visit(ast::Binary* expr) override;

private:
    Value evaluate(ast::Expression* expr);

    bool is_true(const Value& val);
    bool is_equal(const Value& val1, const Value& val2);

    void check_number_operands(const types::Token& op, const Value& operand);
    void check_number_operands(const types::Token& op, const Value& left, const Value& right);
};

}

#endif  // INTERPRETER_H