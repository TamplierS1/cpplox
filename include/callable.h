#ifndef CALLABLE_H
#define CALLABLE_H

#include <vector>

namespace cpplox
{
class Interpreter;
class Value;

// Represents any cpplox object that can be called like a function
class Callable
{
public:
    virtual ~Callable() = default;
    virtual Value call(Interpreter* interpreter, const std::vector<Value>& args) = 0;
    // returns number of arguments
    [[nodiscard]] virtual unsigned int arity() const = 0;
};

}

#endif  // CALLABLE_H