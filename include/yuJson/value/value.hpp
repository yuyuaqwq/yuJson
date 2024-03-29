#ifndef YUJSON_VALUE_VALUE_HPP_
#define YUJSON_VALUE_VALUE_HPP_

#include <stdexcept>

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

class ValueTypeError : public std::runtime_error {
public:
    using Base = std::runtime_error;
public:
    using Base::Base;
};

class NullValue;
class BooleanValue;
#ifndef YUJSON_DISABLE_FLOAT
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
    virtual ValueType Type() const noexcept = 0;

    template <typename T>
    T& To() {
        return *reinterpret_cast<T*>(this);
    }

    bool IsNull() const noexcept {
        return Type() == ValueType::kNull;
    }

    NullValue& ToNull() {
        if (!IsNull()) {
            throw ValueTypeError("Not Null data");
        }
        return *reinterpret_cast<NullValue*>(this);
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
#ifndef YUJSON_DISABLE_FLOAT
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
        return GetArray();
    }

    bool IsObject() const noexcept {
        return Type() == ValueType::kObject;
    }

    ObjectValue& ToObject() {
        if (!IsObject()) {
            throw ValueTypeError("Not Object data");
        }
        return GetObject();
    }


    BooleanValue& GetBoolean() {
        return reinterpret_cast<BooleanValue&>(*this);
    }

    NumberIntValue& GetNumberInt() {
        return reinterpret_cast<NumberIntValue&>(*this);
    }

#ifndef YUJSON_DISABLE_FLOAT
    NumberFloatValue& GetNumberFloat() {
        return reinterpret_cast<NumberFloatValue&>(*this);
    }
#endif

    StringValue& GetString() {
        return reinterpret_cast<StringValue&>(*this);
    }

    ArrayValue& GetArray() {
        return reinterpret_cast<ArrayValue&>(*this);
    }

    ObjectValue& GetObject() {
        return reinterpret_cast<ObjectValue&>(*this);
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

using ValuePtr = std::unique_ptr<ValueInterface>;
using ValuePtrVector = std::vector<ValuePtr>;
using ValuePtrtMap = std::map<std::string, ValuePtr>;

}
}


#include <yuJson/value/null.hpp>
#include <yuJson/value/boolean.hpp>
#include <yuJson/value/number_int.hpp>
#ifndef YUJSON_DISABLE_FLOAT
#include <yuJson/value/number_float.hpp>
#endif
#include <yuJson/value/string.hpp>
#include <yuJson/value/array.hpp>
#include <yuJson/value/object.hpp>
#endif // YUJSON_VALUE_VALUE_HPP_