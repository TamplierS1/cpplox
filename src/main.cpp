#include "scanner.h"
#include "ast_printer.h"
#include "token.h"

int main(int argc, char** argv)
{
    using namespace garm;

    ast::Literal left{static_cast<Value>(123)};
    auto* op = new Token(TokenType::MINUS, "-", std::nullopt, 1);
    ast::Literal right{static_cast<Value>(123)};

    ast::Expression* expr1 = new ast::Binary(&left, op, &right);

    AstPrinter printer;

    std::cout << printer.print(expr1);

    /*
    garm::Scanner scanner;

    if (argc > 2)
    {
        std::cout << "Usage: garm [script]" << '\n';
        std::exit(64);
    }
    else if (argc == 2)
    {
        scanner.run_file(argv[1]);
    }
    else
    {
        scanner.run_prompt();
    }
*/
    return 0;
}
