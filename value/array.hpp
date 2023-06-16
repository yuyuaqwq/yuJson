#ifndef YUJSON_VALUE_ARRAY_H_
#define YUJSON_VALUE_ARRAY_H_

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {
class ArrayValue : public ValueBase {
public:
    ArrayValue() noexcept { }
    ArrayValue(ArrayValue&& arr) noexcept : m_arr(std::move(arr.m_arr)) { }
    ~ArrayValue() noexcept { }

    ArrayValue(const ArrayValue&) = delete;
    void operator=(const ArrayValue&) = delete;

    virtual ValueType Type() const noexcept {
        return ValueType::kArray;
    }
    const ValueVector& GetVector() const noexcept {
        return m_arr;
    }

    ValueBase& Get(int i) {
        if (i < 0 || i >= m_arr.size()) {
            return *(ValueBase*)nullptr;
        }
        return (ValueBase&)*m_arr[i];
    }

    Value* GetPtr(int i) {
        if (i < 0 || i >= m_arr.size()) {
            return nullptr;
        }
        return &m_arr[i];
    }

    void Pushback(Value value) {
        m_arr.push_back(std::move(value));
    }

    void Pushback(nullptr_t) {
        Pushback(_SCN make_unique<NullValue>());
    }

    void Pushback(BooleanValue&& boolean) {
        Pushback(_SCN make_unique<BooleanValue>(std::move(boolean)));
    }

    void Pushback(NumberValue&& num) {
        Pushback(_SCN make_unique<NumberValue>(std::move(num)));
    }

    void Pushback(StringValue&& str) {
        Pushback(_SCN make_unique<StringValue>(std::move(str)));
    }

    void Pushback(ArrayValue&& arr) {
        Pushback(_SCN make_unique<ArrayValue>(std::move(arr)));
    }

    void Pushback(ObjectValue&& obj);

    void Set(int i, _SCN unique_ptr<ValueBase> value) noexcept {
        m_arr[i] = std::move(value);
    }

    void Set(int i, nullptr_t) noexcept {
        Set(i, _SCN make_unique<NullValue>());
    }

    void Set(int i, BooleanValue&& boolean) noexcept {
        Set(i, _SCN make_unique<BooleanValue>(std::move(boolean)));
    }

    void Set(int i, NumberValue&& num) noexcept {
        Set(i, _SCN make_unique<NumberValue>(std::move(num)));
    }

    void Set(int i, StringValue&& str) noexcept {
        Set(i, _SCN make_unique<StringValue>(std::move(str)));
    }

    void Set(int i, ArrayValue&& arr) noexcept {
        Set(i, _SCN make_unique<ArrayValue>(std::move(arr)));
    }

    void Set(int i, ObjectValue&& obj) noexcept;

private:
    ValueVector m_arr;
};

using Array = _SCN unique_ptr<ArrayValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_ARRAY_H_