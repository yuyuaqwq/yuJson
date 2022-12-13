#ifndef AST_NULL_H_
#define AST_NULL_H_

#include "ast\value.hpp"

namespace yuJson {
namespace ast {

class Null : public Value {
public:
	virtual ValueType Type() const noexcept {
		return ValueType::kNull;
	}
};

} // namespace ast
} // namespace yuJson

#endif // AST_NULL_H_