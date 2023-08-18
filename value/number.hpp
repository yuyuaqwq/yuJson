#ifndef YUJSON_VALUE_NUMBER_H_
#define YUJSON_VALUE_NUMBER_H_

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {
class NumberValue : public ValueInterface {
public:
#ifdef YUJSON_ENABLE_FLOAT
    explicit NumberValue(double num) noexcept : m_float(num) { }
#endif
    explicit NumberValue(long long num) noexcept : m_int(num) { }
    explicit NumberValue(unsigned long long num) noexcept : m_int((long long)num) { }
    
    ~NumberValue() noexcept { }

    virtual ValueType Type() const noexcept {
        return ValueType::kNumber;
    }

#ifdef YUJSON_ENABLE_FLOAT
    double& GetFloat() noexcept {
        return m_float;
    }

    void SetFloat(double num) noexcept {
        m_float = num;
    }
#endif

    long long& GetInt() noexcept {
        return m_int;
    }

    void SetInt(long long num) noexcept {
        m_int = num;
    }

private:
    union
    {
#ifdef YUJSON_ENABLE_FLOAT
        double m_float;
#endif
        long long m_int;
    };

};

using NumberPtr = YUJSON_STD unique_ptr<NumberValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_NUMBER_H_