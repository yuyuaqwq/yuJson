#ifndef YUJSON_AST_ARRAY_H_
#define YUJSON_AST_ARRAY_H_

#include <vector>
#include <memory>

#include "ast\value.hpp"

namespace yuJson {
namespace ast {

class Array : public Value {
public:
	Array() noexcept { }
	~Array() noexcept { }

	Array(const Array&) = delete;
	void operator=(const Array&) = delete;

	virtual ValueType Type() const noexcept {
		return ValueType::kArray;
	}

	const std::vector<std::unique_ptr<Value>>& Get() const noexcept {
		return m_arr;
	}

	Value& At(int i) {
		return *m_arr.at(i);
	}

	void PushBack(std::unique_ptr<Value> value) {
		m_arr.push_back(std::move(value));
	}

	void Set(int i, std::unique_ptr<Value> value) noexcept {
		m_arr[i] = std::move(value);
	}

private:
	std::vector<std::unique_ptr<Value>> m_arr;
};

} // namespace ast
} // namespace yuJson

#endif // YUJSON_AST_ARRAY_H_