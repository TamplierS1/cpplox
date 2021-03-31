 target_sources(cpplox PRIVATE main.cpp scanner.cpp error_handler.cpp token.cpp value.cpp parser.cpp interpreter.cpp environment.cpp function.cpp)

 add_subdirectory(native_functions)