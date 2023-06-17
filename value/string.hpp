#ifndef YUJSON_VALUE_STRING_H_
#define YUJSON_VALUE_STRING_H_

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {
class StringValue : public ValueBase {
public:
    explicit StringValue(const char* str) : m_str(str) { }
    explicit StringValue(const _SCN string& str) : m_str(str) { }
    StringValue(StringValue&& str) noexcept : m_str(str.Get()) { }

    void operator=(StringValue& str) = delete;

    void operator=(StringValue&& str) noexcept {
        this->m_str = std::move(str.Get());
    }

    ~StringValue() noexcept { }

    virtual ValueType Type() const noexcept {
        return ValueType::kString;
    }

    _SCN string& Get() noexcept {
        return m_str;
    }

private:
    _SCN string m_str;
};

using StringPtr = _SCN unique_ptr<StringValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_STRING_H_