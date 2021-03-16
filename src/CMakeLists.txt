 add_compile_options(-Wall -Wextra -pedantic -Wno-narrowing -Wno-sign-compare -O1)

 add_executable(garm main.cpp scanner.cpp error_handler.cpp token.cpp literal.cpp)