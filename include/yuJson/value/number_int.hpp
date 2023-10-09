#ifndef YUJSON_VALUE_NUMBER_INT_HPP_
#define YUJSON_VALUE_NUMBER_INT_HPP_

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {
class NumberIntValue : public ValueInterface {
public:
    explicit NumberIntValue(long long num) noexcept : int_(num) { }
    explicit NumberIntValue(unsigned long long num) noexcept : int_((long long)num) { }
    
    ~NumberIntValue() noexcept { }

    ValueType Type() const override noexcept {
        return ValueType::kNumberInt;
    }

    long long& Get() noexcept {
        return int_;
    }

    void Set(long long num) noexcept {
        int_ = num;
    }

private:
    long long int_;
};

using NumberIntPtr = std::unique_ptr<NumberIntValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_NUMBER_INT_HPP_