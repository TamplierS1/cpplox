#include "native_functions/clock_fn.h"

namespace cpplox
{
Value ClockFunction::call(Interpreter *interpreter, const std::vector<Value>& args)
{
    using namespace std::chrono;
    auto now_ms = time_point_cast<milliseconds>(system_clock::now());
    auto value = now_ms.time_since_epoch();
    return static_cast<double>(value.count());
}

unsigned int ClockFunction::arity() const
{
    // `clock()` receives 0 arguments, thus we return 0
    return 0;
}

std::string ClockFunction::to_string() const
{
    return "<fn clock>";
}

}