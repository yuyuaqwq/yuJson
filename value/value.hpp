#ifndef YUJSON_VALUE_VALUE_HPP_
#define YUJSON_VALUE_VALUE_HPP_

#include <exception>

namespace yuJson {
namespace value {

enum class ValueType {
    kNull = 0,
    kBoolean,
    kNumberInt,
    kNumberFloat,
    kString,
    kArray,
    kObject
};

class ValueTypeError : public std::exception {
public:
    ValueTypeError(const char* message) : std::exception(message) {

    }
};

class NullValue;
class BooleanValue;
#ifnde YUJSON_DISABLE_FLOAT
class NumberFloatValue;
#endif
class NumberIntValue;
class StringValue;
class ArrayValue;
class ObjectValue;

class ValueInterface {
protected:
    ValueInterface() noexcept { }

public:
    virtual ~ValueInterface() noexcept { }

public:
    virtual inline ValueType Type() const noexcept = 0;

    template <typename T>
    T& To() {
        return *(T*)this;// static_cast<ast::T*>(this);
    }

    bool IsNull() const noexcept {
        return Type() == ValueType::kNull;
    }

    NullValue& ToNull() {
        if (!IsNull()) {
            throw ValueTypeError("Not Null data");
        }
        return *(NullValue*)this;// static_cast<ast::Boolean*>(this);
    }

    bool IsBoolean() const noexcept {
        return Type() == ValueType::kBoolean;
    }


    BooleanValue& ToBoolean() {
        if (!IsBoolean()) {
            throw ValueTypeError("Not Boolean data");
        }
        return GetBoolean();
    }

    bool IsNumberInt() const noexcept {
        return Type() == ValueType::kNumberInt;
    }

    bool IsNumberFloat() const noexcept {
        return Type() == ValueType::kNumberFloat;
    }

    NumberIntValue& ToNumberInt() {
        if (!IsNumberInt()) {
            throw ValueTypeError("Not Number data");
        }
        return GetNumberInt();
    }
#ifnde YUJSON_DISABLE_FLOAT
    NumberFloatValue& ToNumberFloat() {
        if (!IsNumberFloat()) {
            throw ValueTypeError("Not Number data");
        }
        return GetNumberFloat();
    }
#endif
    bool IsString() const noexcept {
        return Type() == ValueType::kString;
    }

    StringValue& ToString() {
        if (!IsString()) {
            throw ValueTypeError("Not String data");
        }
        return GetString();
    }

    bool IsArray() const noexcept {
        return Type() == ValueType::kArray;
    }

    ArrayValue& ToArray() {
        if (!IsArray()) {
            throw ValueTypeError("Not Array data");
        }
        return *(ArrayValue*)this; //static_cast<ast::Array*>(this);
    }

    bool IsObject() const noexcept {
        return Type() == ValueType::kObject;
    }

    ObjectValue& ToObject() {
        if (!IsObject()) {
            throw ValueTypeError("Not Object data");
        }
        return *(ObjectValue*)this; // static_cast<ast::Object*>(this);
    }


    BooleanValue& GetBoolean() {
        return *(BooleanValue*)this;// static_cast<ast::Boolean*>(this);
    }

    NumberIntValue& GetNumberInt() {
        return *(NumberIntValue*)this; // static_cast<ast::Number*>(this);
    }

#ifnde YUJSON_DISABLE_FLOAT
    NumberFloatValue& GetNumberFloat() {
        return *(NumberFloatValue*)this; // static_cast<ast::Number*>(this);
    }
#endif

    StringValue& GetString() {
        return *(StringValue*)this; // static_cast<ast::String*>(this);
    }


};

} // namespace value
} // namespace yuJson


#include <vector>
#include <map>
#include <string>
#include <memory>
namespace yuJson{
namespace value{

using ValuePtr = YUJSON_STD unique_ptr<ValueInterface>;
using ValuePtrVector = YUJSON_STD vector<ValuePtr>;
using ValuePtrtMap = YUJSON_STD map<YUJSON_STD string, ValuePtr>;

}
}


#include <yuJson/value/null.hpp>
#include <yuJson/value/boolean.hpp>
#include <yuJson/value/number_int.hpp>
#ifnde YUJSON_DISABLE_FLOAT
#include <yuJson/value/number_float.hpp>
#endif
#include <yuJson/value/string.hpp>
#include <yuJson/value/array.hpp>
#include <yuJson/value/object.hpp>
#endif // YUJSON_VALUE_VALUE_HPP_