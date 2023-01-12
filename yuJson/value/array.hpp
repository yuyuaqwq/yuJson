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

	Array(Array&& arr) noexcept : m_arr(std::move(arr.m_arr)) { }

	~Array() noexcept { }

	Array(const Array&) = delete;
	void operator=(const Array&) = delete;

	virtual ValueType Type() const noexcept {
		return ValueType::kArray;
	}

	const std::vector<std::unique_ptr<Value>>& GetVector() const noexcept {
		return m_arr;
	}

	Value& Get(int i) {
		return *m_arr.at(i);
	}

	void Pushback(std::unique_ptr<Value> value) {
		m_arr.push_back(std::move(value));
	}

	void Pushback(nullptr_t) {
		Pushback(std::make_unique<Null>());
	}

	void Pushback(Boolean&& boolean) {
		Pushback(std::make_unique<Boolean>(std::move(boolean)));
	}

	void Pushback(Number&& num) {
		Pushback(std::make_unique<Number>(std::move(num)));
	}

	void Pushback(String&& str) {
		Pushback(std::make_unique<String>(std::move(str)));
	}

	void Pushback(Array&& arr) {
		Pushback(std::make_unique<Array>(std::move(arr)));
	}

	void Pushback(Object&& obj);

	void Set(int i, std::unique_ptr<Value> value) noexcept {
		m_arr[i] = std::move(value);
	}

	void Set(int i, nullptr_t) noexcept {
		Set(i, std::make_unique<Null>());
	}

	void Set(int i, Boolean&& boolean) noexcept {
		Set(i, std::make_unique<Boolean>(std::move(boolean)));
	}

	void Set(int i, Number&& num) noexcept {
		Set(i, std::make_unique<Number>(std::move(num)));
	}

	void Set(int i, String&& str) noexcept {
		Set(i, std::make_unique<String>(std::move(str)));
	}

	void Set(int i, Array&& arr) noexcept {
		Set(i, std::make_unique<Array>(std::move(arr)));
	}

	void Set(int i, Object&& obj) noexcept;

private:
	std::vector<std::unique_ptr<Value>> m_arr;
};

} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_ARRAY_H_