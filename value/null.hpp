#ifndef YUJSON_VALUE_NULL_H_
#define YUJSON_VALUE_NULL_H_

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {
class NullValue : public ValueInterface {
public:
    NullValue() { }

    NullValue(std::nullptr_t) { }

    virtual ValueType Type() const noexcept {
        return ValueType::kNull;
    }
};

using NullPtr = YUJSON_STD unique_ptr<NullValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_NULL_H_