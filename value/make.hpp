#ifndef YUJSON_VALUE_MAKE_H_
#define YUJSON_VALUE_MAKE_H_

#include <yuJson/value/value.hpp>

namespace yuJson {
  namespace value {
    // ����Number��C����
#define ALL_NUMBER_TYPES char, signed char, unsigned char, wchar_t, char16_t, char32_t, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long, float, double, long double

    // ���JudgedObjT��JudgeTypesT, �򷵻�����ΪRetValT
    template<class JudgedObjT, class RetValT, class... JudgeTypesT>
    using enable_if_any_of_t = typename std::enable_if_t<std::_Is_any_of_v<std::remove_reference_t<JudgedObjT>, JudgeTypesT...>, RetValT>;

    // ���TΪbool, ����BooleanValue
    template<class T>
    using enable_if_bool_t = enable_if_any_of_t<T, BooleanPtr, bool>;
    // ���T��ALL_NUMBER_TYPES, ����NumberValue
    template<class T>
    using enable_if_number_t = enable_if_any_of_t<T, NumberPtr, ALL_NUMBER_TYPES>;
    template<class T>
    // ���TΪnullptr, ����NullValue
    using enable_if_null_t = typename std::enable_if_t<std::is_null_pointer_v<T>, NullPtr>;
    // ���T��{const char*, const char[N]}, ����StringValue
    template<class T>
    using enable_if_str_t = enable_if_any_of_t<T, StringPtr, const char*, const char[]>;

    /*
    * ����enable_if������, ���ݲ�ͬ���͵�T���ò�ͬ��"����"����
    * ���û��ƥ�����ᱨ��
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