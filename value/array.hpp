#ifndef YUJSON_VALUE_ARRAY_H_
#define YUJSON_VALUE_ARRAY_H_

#include <stdexcept>

#include <yuJson/value/value.hpp>

namespace yuJson {
class Json;
namespace value {
class ArrayValue : public ValueInterface {
public:
    ArrayValue() noexcept { }
    ArrayValue(ArrayValue&& arr) noexcept : m_arr(std::move(arr.m_arr)) { }
    ~ArrayValue() noexcept { }

    ArrayValue(const ArrayValue&) = delete;
    void operator=(const ArrayValue&) = delete;

    virtual ValueType Type() const noexcept {
        return ValueType::kArray;
    }
    const ValuePtrVector& GetVector() const noexcept {
        return m_arr;
    }

    ValuePtr& At(int i) {
        return m_arr.at(i);
    }

    ValuePtr& operator[](int i) {
        return m_arr[i];
    }

    void Pushback(ValuePtr&& value) {
        m_arr.push_back(std::move(value));
    }

    void Set(int i, ValuePtr value) noexcept {
        m_arr[i] = std::move(value);
    }

private:
    ValuePtrVector m_arr;
};

using ArrayPtr = YUJSON_STD unique_ptr<ArrayValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_ARRAY_H_