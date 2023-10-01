#ifndef YUJSON_VALUE_STRING_HPP_
#define YUJSON_VALUE_STRING_HPP_

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {
class StringValue : public ValueInterface {
public:
    explicit StringValue(const std::string& str) : str_(str) { }
    StringValue(StringValue&& str) noexcept : str_(str.Get()) { }

    void operator=(StringValue& str) = delete;

    void operator=(StringValue&& str) noexcept {
        this->str_ = std::move(str.Get());
    }

    ~StringValue() noexcept { }

    virtual ValueType Type() const noexcept {
        return ValueType::kString;
    }

    std::string& Get() noexcept {
        return str_;
    }

private:
    std::string str_;
};

using StringPtr = std::unique_ptr<StringValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_STRING_HPP_