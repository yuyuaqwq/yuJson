#ifndef YUJSON_VALUE_NULL_HPP_
#define YUJSON_VALUE_NULL_HPP_

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {
class NullValue : public ValueInterface {
public:
    NullValue() { }

    explicit NullValue(std::nullptr_t) { }

    virtual ValueType Type() const noexcept {
        return ValueType::kNull;
    }
};

using NullPtr = YUJSON_STD unique_ptr<NullValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_NULL_HPP_