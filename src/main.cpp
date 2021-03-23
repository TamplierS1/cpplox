#include "ast_printer.h"
#include "scanner.h"
#include "parser.h"
#include "token.h"

int main(int argc, char** argv)
{
    using namespace garm;

    Scanner scanner;
    AstPrinter printer;

    if (argc > 2)
    {
        std::cout << "Usage: garm [script]" << '\n';
        std::exit(64);
    }
    else if (argc == 2)
    {
        auto tokens = scanner.run_file(argv[1]);
        if(!tokens.has_value())
        {
            std::exit(-1);
        }

        Parser parser{tokens.value()};
        ExpressionPtr expr = parser.parse().value();

        /*std::cout << dynamic_cast<ast::Binary*>(expr.get())->m_right << '\n';
        std::cout << dynamic_cast<ast::Binary*>(expr.get())->m_op << '\n';
        std::cout << dynamic_cast<ast::Binary*>(expr.get())->m_left << '\n';
         */
        if (ErrorHandler::get_instance().m_had_error)
            std::exit(-1);

        std::cout << printer.print(expr.get());
    }

    return 0;
}
