#ifndef YUJSON_VALUE_ARRAY_HPP_
#define YUJSON_VALUE_ARRAY_HPP_

#include <stdexcept>

#include <yuJson/value/value.hpp>

namespace yuJson {
class Json;
namespace value {
class ArrayValue : public ValueInterface {
public:
    ArrayValue() noexcept { }
    ArrayValue(ArrayValue&& arr) noexcept : arr_(std::move(arr.arr_)) { }
    ~ArrayValue() noexcept { }

    ArrayValue(const ArrayValue&) = delete;
    void operator=(const ArrayValue&) = delete;

    ValueType Type() const noexcept override {
        return ValueType::kArray;
    }
    ValuePtrVector& GetVector() noexcept {
        return arr_;
    }

    ValuePtr& At(int i) {
        return arr_.at(i);
    }

    ValuePtr& operator[](int i) {
        return arr_[i];
    }

    void Pushback(ValuePtr value) {
        arr_.push_back(std::move(value));
    }

    void Set(int i, ValuePtr value) noexcept {
        arr_[i] = std::move(value);
    }

private:
    ValuePtrVector arr_;
};

using ArrayPtr = std::unique_ptr<ArrayValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_ARRAY_HPP_