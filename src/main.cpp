#include "scanner.h"

int main(int argc, char** argv)
{
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

    return 0;
}
