#ifndef YUJSON_VALUE_OBJECT_H_
#define YUJSON_VALUE_OBJECT_H_

#include <map>
#include <string>
#include <memory>

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {

class Object : public Value {
public:
	Object() noexcept { }

	~Object() noexcept { }

	Object(Object&& obj) noexcept : m_obj(std::move(obj.m_obj)) { }

	Object(const Object&) = delete;
	void operator=(const Object&) = delete;

	virtual ValueType Type() const noexcept {
		return ValueType::kObject;
	}

	const std::map<std::string, std::unique_ptr<Value>>& GetMap() const noexcept {
		return m_obj;
	}

	Value& Get(const std::string& key) {
		return *m_obj.at(key);
	}

	bool Find(const std::string& key) const noexcept {
		return m_obj.find(key) != m_obj.end();
	}

	void Set(const std::string& key, std::unique_ptr<Value> value) {
		auto& it = m_obj.operator[](key);
		it = std::move(value);
	}

	void Set(const std::string& key, nullptr_t) {
		Set(key, std::make_unique<value::Null>());
	}

	void Set(const std::string& key, Boolean&& boolean) {
		Set(key, std::make_unique<value::Boolean>(std::move(boolean)));
	}

	void Set(const std::string& key, Number&& num) {
		Set(key, std::make_unique<value::Number>(std::move(num)));
	}

	void Set(const std::string& key, String&& str) {
		Set(key, std::make_unique<value::String>(std::move(str)));
	}

	void Set(const std::string& key, Array&& arr) {
		Set(key, std::make_unique<value::Array>(std::move(arr)));
	}

	void Set(const std::string& key, Object&& obj) {
		Set(key, std::make_unique<value::Object>(std::move(obj)));
	}

	void Delete(const std::string& key) noexcept {
		m_obj.erase(key);
	}

private:
	std::map<std::string, std::unique_ptr<Value>> m_obj;
};

inline void Array::Pushback(Object&& obj) {
	Pushback(std::make_unique<Object>(std::move(obj)));
}

inline void Array::Set(int i, Object&& obj) noexcept {
	Set(i, std::make_unique<Object>(std::move(obj)));
}

} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_OBJECT_H_