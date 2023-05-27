#ifndef YUJSON_VALUE_VALUE_H_
#define YUJSON_VALUE_VALUE_H_

#include <exception>

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

class Null;
class Boolean;
class Number;
class String;
class Array;
class Object;

class Value {
public:
    class TypeError : public std::exception {
    public:
        TypeError(const char* message) : std::exception(message){

        }
    };

public:
    Value() noexcept { }
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

    Null& ToNull() {
        if (!IsNull()) {
            throw TypeError("Not Null data");
        }
        return *(Null*)this;// static_cast<ast::Boolean*>(this);
    }

    bool IsBoolean() const noexcept {
        return Type() == ValueType::kBoolean;
    }

    Boolean& ToBoolean() {
        if (!IsBoolean()) {
            throw TypeError("Not Boolean data");
        }
        return *(Boolean*)this;// static_cast<ast::Boolean*>(this);
    }

    bool IsNumber() const noexcept {
        return IsValid() && Type() == ValueType::kNumber;
    }

    Number& ToNumber() {
        if (!IsNumber()) {
            throw TypeError("Not Number data");
        }
        return *(Number*)this; // static_cast<ast::Number*>(this);
    }

    bool IsString() const noexcept {
        return IsValid() && Type() == ValueType::kString;
    }

    String& ToString() {
        if (!IsString()) {
            throw TypeError("Not String data");
        }
        return *(String*)this; // static_cast<ast::String*>(this);
    }

    bool IsArray() const noexcept {
        return IsValid() && Type() == ValueType::kArray;
    }

    Array& ToArray() {
        if (!IsArray()) {
            throw TypeError("Not Array data");
        }
        return *(Array*)this; //static_cast<ast::Array*>(this);
    }

    bool IsObject() const noexcept {
        return IsValid() && Type() == ValueType::kObject;
    }

    Object& ToObject() {
        if (!IsObject()) {
            throw TypeError("Not Object data");
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