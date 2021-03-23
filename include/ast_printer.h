#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include <string>
#include <variant>
#include <vector>
#include <memory>

#include "syntax_tree/binary.h"
#include "syntax_tree/grouping.h"
#include "syntax_tree/literal.h"
#include "syntax_tree/unary.h"
#include "error_handler.h"

namespace garm
{

// Prints a syntax tree for a given expression
class AstPrinter : public ast::Visitor
{
public:
    std::string print(ast::Expression* expr);

    Value visit(ast::Binary* expr) override;
    Value visit(ast::Grouping* expr) override;
    Value visit(ast::Literal* expr) override;
    Value visit(ast::Unary* expr) override;
private:
    std::string parenthesize(const std::string& name, std::vector<ExpressionPtr>&& exprs);
};
}

#endif  // AST_PRINTER_H