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

  void Set(const YUJSON_STD string& key, nullptr_t) {
    Set(key, YUJSON_STD make_unique<value::NullValue>());
  }

  void Set(const YUJSON_STD string& key, BooleanValue&& boolean) {
    Set(key, YUJSON_STD make_unique<value::BooleanValue>(std::move(boolean)));
  }

  void Set(const YUJSON_STD string& key, NumberValue&& num) {
    Set(key, YUJSON_STD make_unique<value::NumberValue>(std::move(num)));
  }

  void Set(const YUJSON_STD string& key, StringValue&& str) {
    Set(key, YUJSON_STD make_unique<value::StringValue>(std::move(str)));
  }

  void Set(const YUJSON_STD string& key, ArrayValue&& arr) {
    Set(key, YUJSON_STD make_unique<value::ArrayValue>(std::move(arr)));
  }

  void Set(const YUJSON_STD string& key, ObjectValue&& obj) {
    Set(key, YUJSON_STD make_unique<value::ObjectValue>(std::move(obj)));
  }

  void Delete(const YUJSON_STD string& key) noexcept {
    m_obj.erase(key);
  }

private:
  ValuePtrtMap m_obj;
};

inline void ArrayValue::Pushback(ObjectValue&& obj) {
  Pushback(YUJSON_STD make_unique<ObjectValue>(std::move(obj)));
}

inline void ArrayValue::Set(int i, ObjectValue&& obj) noexcept {
  Set(i, YUJSON_STD make_unique<ObjectValue>(std::move(obj)));
}

using ObjectPtr = YUJSON_STD unique_ptr<ObjectValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_OBJECT_H_