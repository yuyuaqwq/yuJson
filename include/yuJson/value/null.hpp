#ifndef YUJSON_VALUE_NULL_HPP_
#define YUJSON_VALUE_NULL_HPP_

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {
class NullValue : public ValueInterface {
public:
    NullValue() { }

    explicit NullValue(std::nullptr_t) { }

    ValueType Type() const noexcept override {
        return ValueType::kNull;
    }
};

using NullPtr = std::unique_ptr<NullValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_NULL_HPP_