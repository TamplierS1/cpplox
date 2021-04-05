#include "native_functions/println.h"

namespace cpplox
{

Value PrintlnFunction::call(Interpreter *interpreter, const std::vector<Value>& args)
{
    fmt::print("{}\n", args[0].to_string());
    return std::nullopt;
}

int PrintlnFunction::arity() const
{
    return 1;
}

std::string PrintlnFunction::to_string() const
{
    return "<fn println>";
}
}