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

class NullValue;
class BooleanValue;
class NumberValue;
class StringValue;
class ArrayValue;
class ObjectValue;

class ValueBase {
public:
  class TypeError : public std::exception {
  public:
    TypeError(const char* message) : std::exception(message){

    }
  };

public:
  ValueBase() noexcept { }
  virtual ~ValueBase() noexcept { }

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
      throw TypeError("Not Null data");
    }
    return *(NullValue*)this;// static_cast<ast::Boolean*>(this);
  }

  bool IsBoolean() const noexcept {
    return Type() == ValueType::kBoolean;
  }

  BooleanValue& ToBoolean() {
    if (!IsBoolean()) {
      throw TypeError("Not Boolean data");
    }
    return *(BooleanValue*)this;// static_cast<ast::Boolean*>(this);
  }

  bool IsNumber() const noexcept {
    return Type() == ValueType::kNumber;
  }

  NumberValue& ToNumber() {
    if (!IsNumber()) {
      throw TypeError("Not Number data");
    }
    return *(NumberValue*)this; // static_cast<ast::Number*>(this);
  }

  bool IsString() const noexcept {
    return Type() == ValueType::kString;
  }

  StringValue& ToString() {
    if (!IsString()) {
      throw TypeError("Not String data");
    }
    return *(StringValue*)this; // static_cast<ast::String*>(this);
  }

  bool IsArray() const noexcept {
    return Type() == ValueType::kArray;
  }

  ArrayValue& ToArray() {
    if (!IsArray()) {
      throw TypeError("Not Array data");
    }
    return *(ArrayValue*)this; //static_cast<ast::Array*>(this);
  }

  bool IsObject() const noexcept {
    return Type() == ValueType::kObject;
  }

  ObjectValue& ToObject() {
    if (!IsObject()) {
      throw TypeError("Not Object data");
    }
    return *(ObjectValue*)this; // static_cast<ast::Object*>(this);
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

using ValuePtr = _SCN unique_ptr<ValueBase>;
using ValuePtrVector = _SCN vector<ValuePtr>;
using ValuePtrtMap = _SCN map<_SCN string, ValuePtr>;

}
}


#include <yuJson/value/null.hpp>
#include <yuJson/value/boolean.hpp>
#include <yuJson/value/number.hpp>
#include <yuJson/value/string.hpp>
#include <yuJson/value/array.hpp>
#include <yuJson/value/object.hpp>
#endif // YUJSON_VALUE_VALUE_H_