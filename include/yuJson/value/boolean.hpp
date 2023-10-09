#ifndef YUJSON_VALUE_BOOLEAN_HPP_
#define YUJSON_VALUE_BOOLEAN_HPP_

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {

class BooleanValue : public ValueInterface {
public:
    explicit BooleanValue(bool b) noexcept : bool_(b) { }
    ~BooleanValue() noexcept { }

    ValueType Type() const override noexcept {
        return ValueType::kBoolean;
    }

    bool& Get() noexcept {
        return bool_;
    }
    
    void Set(bool b) noexcept {
        bool_ = b;
    }

private:
    bool bool_;
};

using BooleanPtr = std::unique_ptr<BooleanValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_BOOLEAN_HPP_