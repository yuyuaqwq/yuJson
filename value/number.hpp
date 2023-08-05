#ifndef YUJSON_VALUE_NUMBER_H_
#define YUJSON_VALUE_NUMBER_H_

#include <yuJson/value/value.hpp>

namespace yuJson {
namespace value {
class NumberValue : public ValueInterface {
public:
  //explicit Number(double num) noexcept : m_float(num) { }
  NumberValue(long long num) noexcept : m_int(num) { }
  ~NumberValue() noexcept { }

  virtual ValueType Type() const noexcept {
    return ValueType::kNumber;
  }

  double& GetFloat() noexcept {
    return m_float;
  }

  void SetFloat(double num) noexcept {
    m_float = num;
  }

  long long& GetInt() noexcept {
    return m_int;
  }

  void SetInt(long long num) noexcept {
    m_int = num;
  }

private:
  union
  {
    double m_float;
    long long m_int;
  };

};

using NumberPtr = YUJSON_STD unique_ptr<NumberValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_NUMBER_H_