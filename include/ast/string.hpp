#ifndef YUJSON_AST_STRING_H_
#define YUJSON_AST_STRING_H_

#include <string>

#include "ast\value.hpp"

namespace yuJson {
namespace ast {

class String : public Value {
public:
	explicit String(const char* str) : m_str(str) { }
	explicit String(const std::string& str) : m_str(str) { }
	~String() noexcept { }

	virtual ValueType Type() const noexcept {
		return ValueType::kString;
	}

	const std::string& Get() const noexcept {
		return m_str;
	}

private:
	std::string m_str;
};

} // namespace ast
} // namespace yuJson

#endif // YUJSON_AST_STRING_H_