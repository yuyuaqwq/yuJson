#ifndef YUJSON_VALUE_STRING_H_
#define YUJSON_VALUE_STRING_H_

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {
class StringValue : public ValueBase {
public:
    explicit StringValue(const char* str) : m_str(str) { }
    explicit StringValue(const CString& str) : m_str(str) { }
    StringValue(StringValue&& str) noexcept : m_str(str.Get()) { }

    void operator=(StringValue& str) = delete;

    void operator=(StringValue&& str) noexcept {
        this->m_str = std::move(str.Get());
    }

    ~StringValue() noexcept { }

    virtual ValueType Type() const noexcept {
        return ValueType::kString;
    }

    CString& Get() noexcept {
        return m_str;
    }

private:
    CString m_str;
};

using String = UniquePtr<StringValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_STRING_H_