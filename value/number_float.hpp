#ifndef YUJSON_VALUE_NUMBER_FLOAT_H_
#define YUJSON_VALUE_NUMBER_FLOAT_H_

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {
class NumberFloatValue : public ValueInterface {
public:
    explicit NumberFloatValue(double num) noexcept : m_float(num) { }
    
    ~NumberFloatValue() noexcept { }

    virtual ValueType Type() const noexcept {
        return ValueType::kNumberFloat;
    }

    double& GetFloat() noexcept {
        return m_float;
    }

    void SetFloat(double num) noexcept {
        m_float = num;
    }

private:
    double m_float;
};

using NumberFloatPtr = YUJSON_STD unique_ptr<NumberFloatValue>;

} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_NUMBER_FLOAT_H_