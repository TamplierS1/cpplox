#ifndef PRINTLN_FN_H
#define PRINTLN_FN_H

#include "fmt/core.h"
#include "callable.h"
#include "value.h"

namespace cpplox
{
/*
 * Prints 'str' with a newline character after it
 *
 * 'str': a string to print
 *
 */
class PrintlnFunction : public Callable
{
public:
    Value call(Interpreter* interpreter, const std::vector<Value>& args) override;
    [[nodiscard]] int arity() const override;
    [[nodiscard]] std::string to_string() const override;
};
}

#endif  // PRINTLN_FN_H
