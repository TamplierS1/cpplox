#include "ast_printer.h"
#include "scanner.h"
#include "parser.h"
#include "token.h"
#include "interpreter.h"

int main(int argc, char** argv)
{
    using namespace garm;

    Scanner scanner;
    Interpreter interpreter;

    if (argc > 2)
    {
        std::cout << "Usage: garm [script]" << '\n';
        std::exit(64);
    }
    else if (argc == 2)
    {
        auto tokens = scanner.run_file(argv[1]);

        Parser parser{tokens.value()};
        std::optional<ExpressionPtr> result = parser.parse();
        if (!result.has_value())
            std::exit(65);

        ExpressionPtr expr = result.value();

        interpreter.interpret(expr.get());
    }
    else
    {
        std::string line;
        while (std::cout << "> " && std::getline(std::cin, line))
        {
            auto tokens = scanner.run_line(line);

            Parser parser{tokens};
            std::optional<ExpressionPtr> result = parser.parse();
            if (!result.has_value())
                continue;

            // TODO: catch exceptions to prevent terminating the REPL
            ExpressionPtr expr = result.value();

            interpreter.interpret(expr.get());

            std::cout << '\n';
        }

    }

    return 0;
}
