#include "class.h"

#include "instance.h"
#include "interpreter.h"

namespace cpplox
{
Value Class::call(Interpreter *interpreter, const std::vector<Value> &args)
{
    auto instance = std::make_shared<Instance>(std::make_shared<Class>(*this));
    // constructor
    std::optional<std::shared_ptr<Function>> initializer = find_method("init");
    if (initializer.has_value())
    {
        initializer.value()->bind(instance)->call(interpreter, args);
    }

    return instance;
}

int Class::arity() const
{
    std::optional<std::shared_ptr<Function>> initializer = find_method("init");
    if (!initializer.has_value())
        return 0;

    return initializer.value()->arity();
}

std::optional<std::shared_ptr<Function>> Class::find_method(const std::string &name) const
{
    if (m_methods.contains(name))
    {
        return m_methods.at(name);
    }

    return std::nullopt;
}

Value Class::get(const Token &name)
{
    auto method = find_method(name.get_lexeme());
    if (method.has_value())
    {
        if (method.value()->m_is_static)
        {
            auto this_ptr = std::make_shared<Instance>(*this);
            return std::dynamic_pointer_cast<Callable>(method.value()->bind(this_ptr));
        }
        else
            throw RuntimeError{name, "Only static methods can be called from a class."};
    }

    throw RuntimeError{name, "Undefined method '" + name.get_lexeme() + "'."};
}

void Class::set(const Token &name, const Value &value)
{
    throw RuntimeError{name, "Can't set properties on a class."};
}

}
