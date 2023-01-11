#ifndef YUJSON_VALUE_STRING_H_
#define YUJSON_VALUE_STRING_H_

#include <string>

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {

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

} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_STRING_H_