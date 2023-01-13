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

    virtual ValueType Type() const noexcept = 0;

    bool IsValid() const noexcept {
        return this != nullptr;
    }

    template <typename T>
    T& To() {
        return *(T*)this;// static_cast<ast::T*>(this);
    }

    bool IsNull() const noexcept {
        return Type() == ValueType::kNull;
    }

    Null& ToNull() noexcept {
        if (!IsNull()) {
            return *(Null*)nullptr;
        }
        return *(Null*)this;// static_cast<ast::Boolean*>(this);
    }

    bool IsBoolean() const noexcept {
        return Type() == ValueType::kBoolean;
    }

    Boolean& ToBoolean() noexcept {
        if (!IsBoolean()) {
            return *(Boolean*)nullptr;
        }
        return *(Boolean*)this;// static_cast<ast::Boolean*>(this);
    }

    bool IsNumber() const noexcept {
        return IsValid() && Type() == ValueType::kNumber;
    }

    Number& ToNumber() noexcept {
        if (!IsNumber()) {
            return *(Number*)nullptr;
        }
        return *(Number*)this; // static_cast<ast::Number*>(this);
    }

    bool IsString() const noexcept {
        return IsValid() && Type() == ValueType::kString;
    }

    String& ToString() noexcept {
        if (!IsString()) {
            return *(String*)nullptr;
        }
        return *(String*)this; // static_cast<ast::String*>(this);
    }

    bool IsArray() const noexcept {
        return IsValid() && Type() == ValueType::kArray;
    }

    Array& ToArray() noexcept {
        if (!IsArray()) {
            return *(Array*)nullptr;
        }
        return *(Array*)this; //static_cast<ast::Array*>(this);
    }

    bool IsObject() const noexcept {
        return IsValid() && Type() == ValueType::kObject;
    }

    Object& ToObject() noexcept {
        if (!IsObject()) {
            return *(Object*)nullptr;
        }
        return *(Object*)this; // static_cast<ast::Object*>(this);
    }

};

inline std::unique_ptr<Boolean> CreateBoolean(Boolean&& boolean) {
    return std::make_unique<Boolean>(std::move(boolean));
}

inline std::unique_ptr<Number> CreateNumber(Number&& num) {
    return std::make_unique<Number>(std::move(num));
}

inline std::unique_ptr<String> CreateString(String&& str) {
    return std::make_unique<String>(std::move(str));
}

inline std::unique_ptr<Array> CreateArray() {
    return std::make_unique<Array>();
}

inline std::unique_ptr<Object> CreateObject() {
    return std::make_unique<Object>();
}

} // namespace value
} // namespace yuJson

#include <yuJson/value/null.hpp>
#include <yuJson/value/boolean.hpp>
#include <yuJson/value/number.hpp>
#include <yuJson/value/string.hpp>
#include <yuJson/value/array.hpp>
#include <yuJson/value/object.hpp>

#endif // YUJSON_VALUE_VALUE_H_