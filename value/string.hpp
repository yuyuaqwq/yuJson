#ifndef YUJSON_VALUE_STRING_H_
#define YUJSON_VALUE_STRING_H_

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {
class StringValue : public ValueInterface {
public:
    explicit StringValue(const YUJSON_STD string& str) : m_str(str) { }
    StringValue(StringValue&& str) noexcept : m_str(str.Get()) { }

    void operator=(StringValue& str) = delete;

    void operator=(StringValue&& str) noexcept {
        this->m_str = std::move(str.Get());
    }

    ~StringValue() noexcept { }

    virtual ValueType Type() const noexcept {
        return ValueType::kString;
    }

    YUJSON_STD string& Get() noexcept {
        return m_str;
    }

private:
    YUJSON_STD string m_str;
};

using StringPtr = YUJSON_STD unique_ptr<StringValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_STRING_H_