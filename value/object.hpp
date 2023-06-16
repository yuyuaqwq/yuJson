#ifndef YUJSON_VALUE_OBJECT_H_
#define YUJSON_VALUE_OBJECT_H_

#include <yuJson/value/value.hpp>

namespace yuJson {
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

    const ValueMap& GetMap() const noexcept {
        return m_obj;
    }

    ValueBase& Get(const _SCN string& key) {
        if (Find(key)) {
            return *m_obj[key];
        }
        return *(ValueBase*)nullptr;
    }

    _SCN unique_ptr<ValueBase>* GetPtr(const _SCN string& key) {
        if (Find(key)) {
            return &m_obj[key];
        }
        return nullptr;
    }

    bool Find(const _SCN string& key) const noexcept {
        return m_obj.find(key) != m_obj.end();
    }

    void Set(const _SCN string& key, _SCN unique_ptr<ValueBase> value) {
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
    ValueMap m_obj;
};

inline void ArrayValue::Pushback(ObjectValue&& obj) {
    Pushback(_SCN make_unique<ObjectValue>(std::move(obj)));
}

inline void ArrayValue::Set(int i, ObjectValue&& obj) noexcept {
    Set(i, _SCN make_unique<ObjectValue>(std::move(obj)));
}

using Object = _SCN unique_ptr<ObjectValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_OBJECT_H_