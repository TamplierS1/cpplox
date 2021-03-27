 add_compile_options(-Wall -Wextra -pedantic -Wno-narrowing -Wno-sign-compare )

 add_executable(cpplox main.cpp scanner.cpp error_handler.cpp token.cpp ast_printer.cpp value.cpp parser.cpp interpreter.cpp environment.cpp)