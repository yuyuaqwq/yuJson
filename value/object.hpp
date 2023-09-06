#ifndef YUJSON_VALUE_OBJECT_H_
#define YUJSON_VALUE_OBJECT_H_

#include <stdexcept>

#include <yuJson/value/value.hpp>

namespace yuJson {
class Json;
namespace value {
class ObjectValue : public ValueInterface {
public:
    ObjectValue() noexcept { }
    ObjectValue(ObjectValue&& obj) noexcept : m_obj(std::move(obj.m_obj)) { }
    ~ObjectValue() noexcept { }

    ObjectValue(const ObjectValue&) = delete;
    void operator=(const ObjectValue&) = delete;

    virtual ValueType Type() const noexcept {
        return ValueType::kObject;
    }

    ValuePtrtMap& GetMap() noexcept {
        return m_obj;
    }

    ValuePtr& At(const YUJSON_STD string& key) {
        return m_obj.at(key);
    }

    ValuePtr& operator[](const YUJSON_STD string& key) {
        return m_obj[key];
    }

    bool Find(const YUJSON_STD string& key) const noexcept {
        return m_obj.find(key) != m_obj.end();
    }

    void Set(const YUJSON_STD string& key, ValuePtr value) {
        auto& it = m_obj.operator[](key);
        it = std::move(value);
    }

    void Delete(const YUJSON_STD string& key) noexcept {
        m_obj.erase(key);
    }

private:
    ValuePtrtMap m_obj;
};

using ObjectPtr = YUJSON_STD unique_ptr<ObjectValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_OBJECT_H_