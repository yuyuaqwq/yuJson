#ifndef YUJSON_VALUE_VALUE_H_
#define YUJSON_VALUE_VALUE_H_

namespace yuJson {
namespace value {

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

	Value() noexcept { }

	Value(String&& str);

	void Value::operator=(String&& str);

	virtual ~Value() noexcept { }

	virtual ValueType Type() const = 0;

	bool IsNull() const noexcept {
		return Type() == ValueType::kNull;
	}

	bool IsBoolean() const noexcept {
		return Type() == ValueType::kBoolean;
	}

	template <typename T>
	T& To() {
		return *(T*)this;// static_cast<ast::Boolean*>(this);
	}

	Boolean& ToBoolean() noexcept {
		if (!IsBoolean()) {
			return *(Boolean*)nullptr;
		}
		return *(Boolean*)this;// static_cast<ast::Boolean*>(this);
	}

	bool IsNumber() const noexcept {
		return Type() == ValueType::kNumber;
	}

	Number& ToNumber() noexcept {
		if (!IsNumber()) {
			return *(Number*)nullptr;
		}
		return *(Number*)this; // static_cast<ast::Number*>(this);
	}

	bool IsString() const noexcept {
		return Type() == ValueType::kString;
	}

	String& ToString() noexcept {
		if (!IsString()) {
			return *(String*)nullptr;
		}
		return *(String*)this; // static_cast<ast::String*>(this);
	}

	bool IsArray() const noexcept {
		return Type() == ValueType::kArray;
	}

	Array& ToArray() noexcept {
		if (!IsArray()) {
			return *(Array*)nullptr;
		}
		return *(Array*)this; //static_cast<ast::Array*>(this);
	}

	bool IsObject() const noexcept {
		return Type() == ValueType::kObject;
	}

	Object& ToObject() noexcept {
		if (!IsObject()) {
			return *(Object*)nullptr;
		}
		return *(Object*)this; // static_cast<ast::Object*>(this);
	}

};

} // namespace value
} // namespace yuJson

#include <yuJson/value/null.hpp>
#include <yuJson/value/boolean.hpp>
#include <yuJson/value/number.hpp>
#include <yuJson/value/string.hpp>
#include <yuJson/value/array.hpp>
#include <yuJson/value/object.hpp>

#endif // YUJSON_VALUE_VALUE_H_