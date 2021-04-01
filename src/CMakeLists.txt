 target_sources(cpplox PRIVATE main.cpp scanner.cpp error_handler.cpp value.cpp parser.cpp interpreter.cpp environment.cpp function.cpp lambda.cpp)

 add_subdirectory(native_functions)