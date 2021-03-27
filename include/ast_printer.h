#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "error_handler.h"
#include "syntax_tree/expression.h"

namespace cpplox
{
using namespace ast;

// Prints a syntax tree for a given expression
class AstPrinter : public expr::Visitor
{
public:
    std::string print(expr::Expression* expr);

    Value visit(expr::Binary* expr) override;
    Value visit(expr::Grouping* expr) override;
    Value visit(expr::Literal* expr) override;
    Value visit(expr::Unary* expr) override;
    Value visit(expr::Variable* expr) override;

private:
    std::string parenthesize(const std::string& name, std::vector<ExpressionPtr>&& exprs);
};
}

#endif  // AST_PRINTER_H