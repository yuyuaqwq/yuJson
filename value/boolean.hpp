#ifndef YUJSON_VALUE_BOOLEAN_H_
#define YUJSON_VALUE_BOOLEAN_H_

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {

class Boolean : public Value {
public:
    explicit Boolean(bool b) noexcept : m_bool(b) { }
    ~Boolean() noexcept { }

    virtual ValueType Type() const noexcept {
        return ValueType::kBoolean;
    }

    bool Get() noexcept {
        return m_bool;
    }
    
    void Set(bool b) noexcept {
        m_bool = b;
    }

private:
    bool m_bool;
};

} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_BOOLEAN_H_