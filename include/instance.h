#ifndef INSTANCE_H
#define INSTANCE_H

#include <unordered_map>

#include "error.h"
#include "token.h"
#include "value.h"

namespace cpplox
{
class Class;

class Instance
{
public:
    virtual ~Instance() = default;

    // We need this constructor to not have to init Instance in Class constructor
    Instance() = default;
    explicit Instance(const std::shared_ptr<Class>& klass)
        : m_class(klass)
    {
    }

    virtual Value get(const Token& name);
    virtual void set(const Token& name, const Value& value);

    [[nodiscard]] std::string to_string() const;

private:
    std::shared_ptr<Class> m_class;
    std::unordered_map<std::string, Value> m_fields;
};

}

#endif  // INSTANCE_H
