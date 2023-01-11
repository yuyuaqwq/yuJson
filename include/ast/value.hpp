#ifndef YUJSON_AST_VALUE_H_
#define YUJSON_AST_VALUE_H_

namespace yuJson {
namespace ast {

enum class ValueType {
	kNull = 0,
	kBoolean,
	kNumber,
	kString,
	kArray,
	kObject
};

class Boolean;
class Number;
class String;
class Array;
class Object;

class Value {
public:
	virtual ~Value() noexcept { }

	virtual ValueType Type() const = 0;

	bool IsNull() const noexcept {
		return Type() == ValueType::kNull;
	}

	bool IsBoolean() const noexcept {
		return Type() == ValueType::kBoolean;
	}
	Boolean* ToBoolean() noexcept {
		if (!IsBoolean())
		{
			return nullptr;
		}
		return (Boolean*)this;// static_cast<ast::Boolean*>(this);
	}

	bool IsNumber() const noexcept {
		return Type() == ValueType::kNumber;
	}
	Number* ToNumber() noexcept {
		if (!IsNumber())
		{
			return nullptr;
		}
		return (Number*)this; // static_cast<ast::Number*>(this);
	}

	bool IsString() const noexcept {
		return Type() == ValueType::kString;
	}
	String* ToString() noexcept {
		if (!IsString())
		{
			return nullptr;
		}
		return (String*)this; // static_cast<ast::String*>(this);
	}

	bool IsArray() const noexcept {
		return Type() == ValueType::kArray;
	}
	Array* ToArray() noexcept {
		if (!IsArray())
		{
			return nullptr;
		}
		return (Array*)this; //static_cast<ast::Array*>(this);
	}

	bool IsObject() const noexcept {
		return Type() == ValueType::kObject;
	}
	Object* ToObject() noexcept {
		if (!IsObject())
		{
			return nullptr;
		}
		return (Object*)this; // static_cast<ast::Object*>(this);
	}

};

} // namespace ast
} // namespace yuJson

#include "ast\null.hpp"
#include "ast\boolean.hpp"
#include "ast\number.hpp"
#include "ast\string.hpp"
#include "ast\array.hpp"
#include "ast\object.hpp"

#endif // YUJSON_AST_VALUE_H_