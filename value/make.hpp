#ifndef YUJSON_VALUE_MAKE_H_
#define YUJSON_VALUE_MAKE_H_

#include <yuJson/value/value.hpp>

namespace yuJson {
  namespace value {
    // 所有Number的C类型
#define ALL_NUMBER_TYPES char, signed char, unsigned char, wchar_t, char16_t, char32_t, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long, float, double, long double

    // 如果JudgedObjT∈JudgeTypesT, 则返回类型为RetValT
    template<class JudgedObjT, class RetValT, class... JudgeTypesT>
    using enable_if_any_of_t = typename std::enable_if_t<std::_Is_any_of_v<std::remove_reference_t<JudgedObjT>, JudgeTypesT...>, RetValT>;

    // 如果T为bool, 返回BooleanValue
    template<class T>
    using enable_if_bool_t = enable_if_any_of_t<T, BooleanPtr, bool>;
    // 如果T∈ALL_NUMBER_TYPES, 返回NumberValue
    template<class T>
    using enable_if_number_t = enable_if_any_of_t<T, NumberPtr, ALL_NUMBER_TYPES>;
    template<class T>
    // 如果T为nullptr, 返回NullValue
    using enable_if_null_t = typename std::enable_if_t<std::is_null_pointer_v<T>, NullPtr>;
    // 如果T∈{const char*, const char[N]}, 返回StringValue
    template<class T>
    using enable_if_str_t = enable_if_any_of_t<T, StringPtr, const char*, const char[]>;

    /*
    * 利用enable_if的特性, 根据不同类型的T调用不同的"重载"函数
    * 如果没有匹配的则会报错
    */
    template<class T>
    inline constexpr enable_if_bool_t<T> make_value(T value) {
      return _SCN make_unique<BooleanValue>(value);
    }
    template<class T>
    inline constexpr enable_if_number_t<T> make_value(T value) {
      return _SCN make_unique<NumberValue>(value);
    }
    template<class T>
    inline constexpr enable_if_null_t<T> make_value(T value) {
      return _SCN make_unique<NullValue>(value);
    }
    template<class T>
    inline constexpr enable_if_str_t<T> make_value(T&& value) {
      return _SCN make_unique<StringValue>(value);
    }
  }
}

#endif // YUJSON_VALUE_MAKE_H_