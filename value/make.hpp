#ifndef YUJSON_VALUE_MAKE_H_
#define YUJSON_VALUE_MAKE_H_

#include <yuJson/value/value.hpp>

namespace yuJson {
	namespace value {
        // 所有Number的C类型
#define ALL_NUMBER_TYPES char, signed char, unsigned char, wchar_t, char16_t, char32_t, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long, float, double, long double


        // 如果JudgedObjT∈JudgeTypesT, 则返回类型为RetValT
        template<class JudgedObjT, class RetValT, class... JudgeTypesT>
        using enable_if_any_of_t = typename std::enable_if_t<std::_Is_any_of_v<std::remove_cv_t<JudgedObjT>, JudgeTypesT...>, RetValT>;

        // 如果T为bool, 返回BooleanValue
        template<class T>
        using enable_if_bool_t = enable_if_any_of_t<T, Boolean, bool>;
        // 如果T∈ALL_NUMBER_TYPES, 返回NumberValue
        template<class T>
        using enable_if_number_t = enable_if_any_of_t<T, Number, ALL_NUMBER_TYPES>;
        template<class T>
        // 如果T为nullptr, 返回NullValue
        using enable_if_null_t = typename std::enable_if_t<std::is_null_pointer_v<T>, Null>;
        // 如果T∈{const char*, char*}, 返回StringValue
        template<class T>
        using enable_if_str_t = enable_if_any_of_t<T, String, const char*, char*>;

        /*
        * 利用enable_if的特性, 根据不同类型的T调用不同的"重载"函数
        * 如果没有匹配的则会报错
        */
        template<class T>
        inline constexpr enable_if_bool_t<T> MakeValue(T& value)
        {
            return MakeUnique<BooleanValue>(value);
        }
        template<class T>
        inline constexpr enable_if_number_t<T> MakeValue(T& value)
        {
            return MakeUnique<NumberValue>(value);
        }
        template<class T>
        inline constexpr enable_if_null_t<T> MakeValue(T& value)
        {
            return MakeUnique<NullValue>(value);
        }
        template<class T>
        inline constexpr enable_if_str_t<T> MakeValue(T& value)
        {
            return MakeUnique<StringValue>(value);
        }


	}
}

#endif // YUJSON_VALUE_MAKE_H_