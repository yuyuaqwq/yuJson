#ifndef YUJSON_VALUE_NUMBER_INT_HPP_
#define YUJSON_VALUE_NUMBER_INT_HPP_

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {
class NumberIntValue : public ValueInterface {
public:
    explicit NumberIntValue(int64_t num) noexcept : int_(num) { }
    explicit NumberIntValue(uint64_t num) noexcept : int_(num) { }
    
    ~NumberIntValue() noexcept { }

    ValueType Type() const noexcept override {
        return ValueType::kNumberInt;
    }

    int64_t& Get() noexcept {
        return int_;
    }

    void Set(int64_t num) noexcept {
        int_ = num;
    }

private:
    int64_t int_;
};

using NumberIntPtr = std::unique_ptr<NumberIntValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_NUMBER_INT_HPP_