#ifndef YUJSON_AST_NULL_H_
#define YUJSON_AST_NULL_H_

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

#endif // YUJSON_AST_NULL_H_