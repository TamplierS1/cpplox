 target_sources(cpplox PRIVATE main.cpp scanner.cpp error.cpp value.cpp parser.cpp interpreter.cpp environment.cpp function.cpp lambda.cpp resolver.cpp class.cpp instance.cpp)

 add_subdirectory(native_functions)
