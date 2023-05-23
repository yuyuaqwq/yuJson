#ifndef YUJSON_VALUE_STRING_H_
#define YUJSON_VALUE_STRING_H_

#include <string>

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {

class String : public Value {
public:
    explicit String(const char* str) : m_str(str) { }
    explicit String(const std::string& str) : m_str(str) { }
    String(String&& str) noexcept : m_str(str.Get()) { }

    void operator=(String& str) = delete;

    void operator=(String&& str) noexcept {
        this->m_str = std::move(str.Get());
    }

    ~String() noexcept { }

    virtual ValueType Type() const noexcept {
        return ValueType::kString;
    }

    std::string& Get() noexcept {
        return m_str;
    }

private:
    std::string m_str;
};


} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_STRING_H_