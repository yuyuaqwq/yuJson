#ifndef YUJSON_AST_OBJECT_H_
#define YUJSON_AST_OBJECT_H_

#include <map>
#include <string>
#include <memory>

#include "ast\value.hpp"

namespace yuJson {
namespace ast {

class Object : public Value {
public:
	Object() noexcept { }

	~Object() noexcept { }

	Object(const Object&) = delete;
	void operator=(const Object&) = delete;

	virtual ValueType Type() const noexcept {
		return ValueType::kObject;
	}

	const std::map<std::string, std::unique_ptr<Value>>& Get() const noexcept {
		return m_obj;
	}

	Value& At(const std::string& key) {
		return *m_obj.at(key);
	}

	bool Find(const std::string& key) const noexcept {
		return m_obj.find(key) != m_obj.end();
	}

	void Set(const std::string& key, std::unique_ptr<Value> value) {
		auto& it = m_obj.at(key);
		it = std::move(value);
	}

	void Insert(const std::string& key, std::unique_ptr<Value> value) {
		m_obj.insert(make_pair(key, std::move(value)));
	}

	void Delete(const std::string& key) noexcept {
		m_obj.erase(key);
	}


private:
	std::map<std::string, std::unique_ptr<Value>> m_obj;
};

} // namespace ast
} // namespace yuJson

#endif // YUJSON_AST_OBJECT_H_