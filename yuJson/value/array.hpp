#ifndef YUJSON_VALUE_ARRAY_H_
#define YUJSON_VALUE_ARRAY_H_

#include <vector>
#include <memory>

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {

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

} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_ARRAY_H_