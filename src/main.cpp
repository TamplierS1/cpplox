#include "interpreter.h"
#include "parser.h"
#include "resolver.h"
#include "scanner.h"
#include "token.h"

int main(int argc, char** argv)
{
    using namespace cpplox;

    Scanner scanner;
    auto interpreter = std::make_shared<Interpreter>();

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
        if (ErrorHandler::get_instance().m_had_error) std::exit(65);

        Resolver resolver{interpreter};
        resolver.resolve(statements.value());

        if (ErrorHandler::get_instance().m_had_error) std::exit(65);

        interpreter->interpret(statements.value());
    }
    else
    {
        std::string line;
        while (std::cout << "> " && std::getline(std::cin, line))
        {
            auto tokens = scanner.run_line(line);

            Parser parser{tokens};
            std::optional<std::vector<StatementPtr>> statements = parser.parse();
            if (ErrorHandler::get_instance().m_had_error) continue;

            Resolver resolver{interpreter};
            resolver.resolve(statements.value());

            if (ErrorHandler::get_instance().m_had_error) continue;

            interpreter->interpret(statements.value());

            std::cout << '\n';
        }
    }

    return 0;
}
