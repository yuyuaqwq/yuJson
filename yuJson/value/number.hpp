#ifndef YUJSON_VALUE_NUMBER_H_
#define YUJSON_VALUE_NUMBER_H_

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {

class Number : public Value {
public:
	// explicit Number(double num) noexcept : m_float(num) { }
	/*explicit*/ Number(long long num) noexcept : m_int(num) { }
	~Number() noexcept { }

	virtual ValueType Type() const noexcept {
		return ValueType::kNumber;
	}

	double GetFloat() noexcept {
		return m_float;
	}

	void SetFloat(double num) noexcept {
		m_float = num;
	}

	long long GetInt() noexcept {
		return m_int;
	}

	void SetInt(long long num) noexcept {
		m_int = num;
	}

private:
	union
	{
		double m_float;
		long long m_int;
	};

};

} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_NUMBER_H_