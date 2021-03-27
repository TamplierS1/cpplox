#include "scanner.h"
#include "parser.h"
#include "token.h"
#include "interpreter.h"

int main(int argc, char** argv)
{
    using namespace cpplox;

    Scanner scanner;
    Interpreter interpreter;

    if (argc > 2)
    {
        std::cout << "Usage: cpplox [script]" << '\n';
        std::exit(64);
    }
    else if (argc == 2)
    {
        auto tokens = scanner.run_file(argv[1]);

        Parser parser{tokens.value()};
        std::optional<std::vector<StatementPtr>> statements = parser.parse();
        if (!statements.has_value())
            std::exit(65);

        interpreter.interpret(statements.value());
    }
    else
    {
        std::string line;
        while (std::cout << "> " && std::getline(std::cin, line))
        {
            auto tokens = scanner.run_line(line);

            Parser parser{tokens};
            std::optional<std::vector<StatementPtr>> statements = parser.parse();
            if (!statements.has_value())
                continue;

            interpreter.interpret(statements.value());

            std::cout << '\n';
        }

    }

    return 0;
}
