#ifndef CLOCK_FN_H
#define CLOCK_FN_H

#include <chrono>

#include "callable.h"
#include "value.h"

namespace cpplox
{
// Returns time passed since epoch.
class ClockFunction : public Callable
{
public:
    Value call(Interpreter* interpreter, const std::vector<Value>& args) override;
    [[nodiscard]] unsigned int arity() const override;
    [[nodiscard]] std::string to_string() const;
};
}

#endif  // CLOCK_FN_H