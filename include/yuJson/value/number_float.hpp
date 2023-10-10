#ifndef YUJSON_VALUE_NUMBER_FLOAT_HPP_
#define YUJSON_VALUE_NUMBER_FLOAT_HPP_

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {
class NumberFloatValue : public ValueInterface {
public:
    explicit NumberFloatValue(double num) noexcept : float_(num) { }
    
    ~NumberFloatValue() noexcept { }

    ValueType Type() const noexcept override {
        return ValueType::kNumberFloat;
    }

    double& Get() noexcept {
        return float_;
    }

    void Set(double num) noexcept {
        float_ = num;
    }

private:
    double float_;
};

using NumberFloatPtr = std::unique_ptr<NumberFloatValue>;

} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_NUMBER_FLOAT_HPP_