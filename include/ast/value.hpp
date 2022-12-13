#ifndef AST_VALUE_H_
#define AST_VALUE_H_

#include <exception>

namespace yuJson {
namespace ast {

class type_error : public std::exception {
public:
	type_error(const char* msg) : std::exception(msg) { }
};

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
	Boolean& ToBoolean() {
		if (!IsBoolean())
		{
			throw type_error("not boolean");
		}
		return *(Boolean*)this;// static_cast<ast::Boolean*>(this);
	}

	bool IsNumber() const noexcept {
		return Type() == ValueType::kNumber;
	}
	Number& ToNumber() {
		if (!IsNumber())
		{
			throw type_error("not number");
		}
		return *(Number*)this; // static_cast<ast::Number*>(this);
	}

	bool IsString() const noexcept {
		return Type() == ValueType::kString;
	}
	String& ToString() {
		if (!IsString())
		{
			throw type_error("not string");
		}
		return *(String*)this; // static_cast<ast::String*>(this);
	}

	bool IsArray() const noexcept {
		return Type() == ValueType::kArray;
	}
	Array& ToArray() {
		if (!IsArray())
		{
			throw type_error("not array");
		}
		return *(Array*)this; //static_cast<ast::Array*>(this);
	}

	bool IsObject() const noexcept {
		return Type() == ValueType::kObject;
	}
	Object& ToObject() {
		if (!IsObject())
		{
			throw type_error("not object");
		}
		return *(Object*)this; // static_cast<ast::Object*>(this);
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

#endif // AST_VALUE_H_