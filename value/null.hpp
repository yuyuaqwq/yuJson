#ifndef YUJSON_VALUE_NULL_H_
#define YUJSON_VALUE_NULL_H_

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {
class NullValue : public ValueBase {
public:
    virtual ValueType Type() const noexcept {
        return ValueType::kNull;
    }
};

using Null = _SCN unique_ptr<NullValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_NULL_H_