#ifndef CLASS_H
#define CLASS_H

#include <memory>
#include <unordered_map>

#include "function.h"
#include "instance.h"

namespace cpplox
{
using MethodsMap = std::unordered_map<std::string, std::shared_ptr<Function>>;

class Class : public Callable, public Instance
{
public:
    Class(const std::string& name, const std::optional<std::shared_ptr<Class>>& superclass, const MethodsMap& methods)
        : m_name(name)
        , m_super(superclass)
        , m_methods(methods)
    {
    }

    // Callable
    Value call(Interpreter* interpreter, const std::vector<Value>& args) override;
    [[nodiscard]] int arity() const override;
    [[nodiscard]] inline std::string to_string() const override
    {
        return "<class " + m_name + ">";
    }
    [[nodiscard]] std::optional<std::shared_ptr<Function>> find_method(const std::string& name) const;

    // Instance
    Value get(const Token& name) override;
    void set(const Token& name, const Value& value) override;

    const std::string m_name;
    const std::optional<std::shared_ptr<Class>> m_super;

private:
    // A bad idea and a violation of some OOP practises,
    // but I'll still go with it :)
    using Instance::to_string;

    MethodsMap m_methods;
};

}

#endif  // CLASS_H