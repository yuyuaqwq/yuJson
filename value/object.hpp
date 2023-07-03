#ifndef YUJSON_VALUE_OBJECT_H_
#define YUJSON_VALUE_OBJECT_H_

#include <stdexcept>

#include <yuJson/value/value.hpp>

namespace yuJson {
class Json;
namespace value {
class ObjectValue : public ValueBase {
public:
    ObjectValue() noexcept { }
    ObjectValue(ObjectValue&& obj) noexcept : m_obj(std::move(obj.m_obj)) { }
    ~ObjectValue() noexcept { }

    ObjectValue(const ObjectValue&) = delete;
    void operator=(const ObjectValue&) = delete;

    virtual ValueType Type() const noexcept {
        return ValueType::kObject;
    }

    const ValuePtrtMap& GetMap() const noexcept {
        return m_obj;
    }

    Json& At(const _SCN string& key) {
        if (Find(key)) {
            return *(Json*)m_obj[key].get();
        }
        throw std::out_of_range("object at");
    }

    ValueBase* GetPtr(const _SCN string& key) {
        if (Find(key)) {
            return m_obj[key].get();
        }
        return nullptr;
    }

    bool Find(const _SCN string& key) const noexcept {
        return m_obj.find(key) != m_obj.end();
    }

    void Set(const _SCN string& key, ValuePtr value) {
        auto& it = m_obj.operator[](key);
        it = std::move(value);
    }

    void Set(const _SCN string& key, nullptr_t) {
        Set(key, _SCN make_unique<value::NullValue>());
    }

    void Set(const _SCN string& key, BooleanValue&& boolean) {
        Set(key, _SCN make_unique<value::BooleanValue>(std::move(boolean)));
    }

    void Set(const _SCN string& key, NumberValue&& num) {
        Set(key, _SCN make_unique<value::NumberValue>(std::move(num)));
    }

    void Set(const _SCN string& key, StringValue&& str) {
        Set(key, _SCN make_unique<value::StringValue>(std::move(str)));
    }

    void Set(const _SCN string& key, ArrayValue&& arr) {
        Set(key, _SCN make_unique<value::ArrayValue>(std::move(arr)));
    }

    void Set(const _SCN string& key, ObjectValue&& obj) {
        Set(key, _SCN make_unique<value::ObjectValue>(std::move(obj)));
    }

    void Delete(const _SCN string& key) noexcept {
        m_obj.erase(key);
    }

private:
    ValuePtrtMap m_obj;
};

inline void ArrayValue::Pushback(ObjectValue&& obj) {
    Pushback(_SCN make_unique<ObjectValue>(std::move(obj)));
}

inline void ArrayValue::Set(int i, ObjectValue&& obj) noexcept {
    Set(i, _SCN make_unique<ObjectValue>(std::move(obj)));
}

using ObjectPtr = _SCN unique_ptr<ObjectValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_OBJECT_H_