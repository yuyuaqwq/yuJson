#ifndef YUJSON_AST_NUMBER_H_
#define YUJSON_AST_NUMBER_H_

#include "ast\value.hpp"

namespace yuJson {
namespace ast {

class Number : public Value {
public:
	explicit Number(double num) noexcept : m_float(num) { }
	// explicit Number(long long num);
	~Number() noexcept { }

	virtual ValueType Type() const noexcept {
		return ValueType::kNumber;
	}

	double GetFloat() noexcept {
		return m_float;
	}

private:
	union
	{
		double m_float;
		long long m_int;
	};

};

} // namespace ast
} // namespace yuJson

#endif // YUJSON_AST_NUMBER_H_