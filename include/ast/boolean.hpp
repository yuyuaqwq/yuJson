#ifndef AST_BOOLEAN_H_
#define AST_BOOLEAN_H_

#include "ast\value.hpp"

namespace yuJson {
namespace ast {

class Boolean : public Value {
public:
	explicit Boolean(bool b) noexcept : m_bool(b) { }
	~Boolean() noexcept { }

	virtual ValueType Type() const noexcept {
		return ValueType::kBoolean;
	}

	bool Get() noexcept {
		return m_bool;
	}

private:
	bool m_bool;
};

} // namespace ast
} // namespace yuJson

#endif // AST_BOOLEAN_H_