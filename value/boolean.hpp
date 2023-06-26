#ifndef YUJSON_VALUE_BOOLEAN_H_
#define YUJSON_VALUE_BOOLEAN_H_

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {

class BooleanValue : public ValueBase {
public:
    BooleanValue(bool b) noexcept : m_bool(b) { }
    ~BooleanValue() noexcept { }

    virtual ValueType Type() const noexcept {
        return ValueType::kBoolean;
    }

    bool& Get() noexcept {
        return m_bool;
    }
    
    void Set(bool b) noexcept {
        m_bool = b;
    }

private:
    bool m_bool;
};

using BooleanPtr = _SCN unique_ptr<BooleanValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_BOOLEAN_H_