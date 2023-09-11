#ifndef YUJSON_VALUE_NUMBER_INT_HPP_
#define YUJSON_VALUE_NUMBER_INT_HPP_

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {
class NumberIntValue : public ValueInterface {
public:
    explicit NumberIntValue(long long num) noexcept : m_int(num) { }
    explicit NumberIntValue(unsigned long long num) noexcept : m_int((long long)num) { }
    
    ~NumberIntValue() noexcept { }

    virtual ValueType Type() const noexcept {
        return ValueType::kNumberInt;
    }

    long long& GetInt() noexcept {
        return m_int;
    }

    void SetInt(long long num) noexcept {
        m_int = num;
    }


private:
    long long m_int;
};

using NumberIntPtr = YUJSON_STD unique_ptr<NumberIntValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_NUMBER_INT_HPP_