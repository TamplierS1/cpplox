#include "interpreter.h"
#include "parser.h"
#include "resolver.h"
#include "scanner.h"
#include "token.h"

using namespace cpplox;

int run_script(const std::string& filename, const std::vector<std::string>& modules_dirs);
// Deletes everything after the dot - 'test.cpplox' becomes 'test'
std::string take_module_name(const std::string& str);
int print_help();

int main(int argc, char** argv)
{
    std::string filename = argv[1];
    std::vector<std::string> dirs;

    for (int i = 2; i < argc; i++)
    {
        dirs.emplace_back(argv[i]);
    }

    if (argc >= 2)
    {
        return run_script(filename, dirs);
    }
    else
        return print_help();
}

int run_script(const std::string& filename, const std::vector<std::string>& modules_dirs)
{
    Scanner scanner;

    auto tokens = scanner.run_file(filename);

    if (!tokens.has_value())
        return 65;

    Parser parser{tokens.value()};
    std::optional<std::vector<StatementPtr>> statements = parser.parse();
    if (ReportError::g_had_error || !statements.has_value())
        return 65;

    std::deque<StatementPtr> stmts_deque;
    for (const auto& stmt : statements.value())
    {
        stmts_deque.push_back(stmt);
    }

    auto interpreter = std::make_shared<Interpreter>(stmts_deque);

    Resolver resolver{interpreter, take_module_name(filename), modules_dirs};
    resolver.resolve(statements.value());

    if (ReportError::g_had_error)
        return 65;

    interpreter->interpret();

    return 0;
}

std::string take_module_name(const std::string& str)
{
    // TODO: it does not work. Fix.
    bool dot_found = false;
    bool slash_found = false;

    int dot_index = 0;
    int slash_index = 0;

    std::for_each(str.rbegin(), str.rend(), [&](const char c) {
        if (c == '.')
        {
            dot_found = true;
        }
        else if (c == '/')
        {
            slash_found = true;
        }

        if (!dot_found)
            dot_index++;
        if (!slash_found)
            slash_index++;
    });

    return str.substr(slash_index + 1, dot_index - 1);
}

int print_help()
{
    std::cout << "Usage: cpplox [script]" << '\n';
    return 64;
}
