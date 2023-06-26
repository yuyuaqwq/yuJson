#ifndef YUJSON_JSON_H_
#define YUJSON_JSON_H_

#ifndef _SCN
#define _SCN std::
#endif // _SCN

#include <string>
#include <memory>
#include <regex>
#include <initializer_list>

#include <yuJson/compiler/parser.hpp>
#include <yuJson/value/make.hpp>

namespace yuJson {

class Json {
public:
    template<class T>
    using enable_if_cant_convert_to_valueptr_t = std::enable_if_t<!std::is_convertible_v<T, value::ValuePtr>, int>;

    Json() noexcept { }
    explicit Json(value::ValuePtr value) noexcept : m_value{ std::move(value) } { }
    Json(Json&& json) noexcept : m_value{ std::move(json.m_value) } { }
    template<class T, enable_if_cant_convert_to_valueptr_t<T> = 0>
    Json(T value) : m_value{ value::make_value(value) } {}
    template<class ThisT, class... RestT, enable_if_cant_convert_to_valueptr_t<ThisT> = 0>
    Json(ThisT&& this_arg, RestT&&... rest_args) {
        _SCN initializer_list<Json> jsons{std::forward<ThisT>(this_arg), std::forward<RestT>(rest_args)...};
        if (jsons.size() % 2 == 0) {
            bool is_obj = true;
            for (auto iter = jsons.begin(); iter != jsons.end(); iter+=2) {
                if (!iter->m_value->IsString()) {
                    is_obj = false;
                    break;
                }
            }
            if (is_obj) {
                m_value = _SCN make_unique<value::ObjectValue>();
                for (auto iter = jsons.begin(); iter != jsons.end(); iter += 2) {
                    _SCN string key = iter->m_value->ToString().Get();
                    m_value->ToObject().Set(key, std::move(((Json*)iter + 1)->m_value));
                }
            }
        }
        if (!IsValid()) {
            m_value = _SCN make_unique<value::ArrayValue>();
            for (auto iter = jsons.begin(); iter != jsons.end(); iter++) {
                m_value->ToArray().Pushback(std::move(((Json*)iter)->m_value));
            }
        }
    }
    ~Json() noexcept { }

    Json(const Json&) = delete;
    void operator=(const Json&) = delete;

    void operator=(Json&& json) noexcept {
        m_value = std::move(json.m_value);
    }

    Json& operator[](const char* str) {
        if (!GetObject().Find(str)) {
            GetObject().Set(str, nullptr);
        }
        return (Json&)*GetObject().GetPtr(str);
    }

    Json& operator[](int index) {
        return (Json&)*GetArray().GetPtr(index);
    }

    bool operator==(const Json&& other) const {
        if (GetType() != other.GetType()) {
            return false;
        }
        switch (GetType()) {
        case value::ValueType::kNull:
            return true;
        case value::ValueType::kBoolean:
            return  m_value->ToBoolean().Get() == other.m_value->ToBoolean().Get();
        case value::ValueType::kNumber:
            return m_value->ToNumber().GetInt() == other.m_value->ToNumber().GetInt();
        case value::ValueType::kString:
            return m_value->ToString().Get() == other.m_value->ToString().Get();
        default:
            return false;
        }
    }

    bool Find(const char* str) {
        return GetObject().Find(str);
    }

    bool IsValid() noexcept {
        return m_value.get() != nullptr;
    }

    bool Parse(const char* jsonText) {
        compiler::Lexer lexer(jsonText);
        compiler::Parser parser(&lexer);
        auto tempbasic_Json = _SCN make_unique<Json>(parser.ParseValue());
        if (!tempbasic_Json->IsValid()) {
            return false;
        }
        m_value = std::move(tempbasic_Json->m_value);
        return true;
    }

    _SCN string Print(bool format = true) const {
        if (!m_value.get()) {
            return "";
        }
        _SCN string jsonStr;
        Print(m_value.get(), format, 0, &jsonStr);
        return jsonStr;
    }

    template <typename T>
    T& Get() noexcept {
        return *(T*)m_value.get();
    }

    value::ValueType GetType() const noexcept {
        return m_value->Type();
    }

    value::NullValue& GetNull() {
        return m_value->ToNull();
    }

    value::BooleanValue& GetBoolean() {
        return m_value->ToBoolean();
    }

    bool& Boolean() {
        return m_value->ToBoolean().Get();
    }

    value::NumberValue& GetNumber() {
        return m_value->ToNumber();
    }

    long long& Int() {
        return m_value->ToNumber().GetInt();
    }

    double& Float() {
        return m_value->ToNumber().GetFloat();
    }

    value::StringValue& GetString() {
        return m_value->ToString();
    }

    _SCN string& String() {
        return m_value->ToString().Get();
    }

    value::ArrayValue& GetArray() {
        return m_value->ToArray();
    }

    value::ObjectValue& GetObject() {
        return m_value->ToObject();
    }

    void Set(_SCN unique_ptr<value::ValueBase> value) {
        m_value = std::move(value);
    }

    template <typename T>
    void Set(T&& val) {
        m_value = _SCN make_unique<T>(std::move(val));
    }

    void Set(nullptr_t) {
        m_value = _SCN make_unique<value::NullValue>();
    }

    void Set(value::BooleanValue&& boolean) {
        m_value = _SCN make_unique<value::BooleanValue>(std::move(boolean));
    }

    void Set(value::NumberValue&& num) {
        m_value = _SCN make_unique<value::NumberValue>(std::move(num));
    }

    void Set(value::StringValue&& str) {
        m_value = _SCN make_unique<value::StringValue>(std::move(str));
    }

    void Set(value::ArrayValue&& arr) {
        m_value = _SCN make_unique<value::ArrayValue>(std::move(arr));
    }

    void Set(value::ObjectValue&& obj) {
        m_value = _SCN make_unique<value::ObjectValue>(std::move(obj));
    }

    static Json MakeObject()
    {
        return Json(std::make_unique<yuJson::value::ObjectValue>());
    }

    static Json MakeArray()
    {
        return Json(std::make_unique<yuJson::value::ArrayValue>());
    }

public:
    class Iterator {
    public:
        Iterator(Json* base){
            m_base = base;
            if (base) {
                if (m_base->GetType() == value::ValueType::kArray) {
                    m_obj_iter = m_base->GetObject().GetMap().begin();
                }
                else if (m_base->GetType() == value::ValueType::kObject) {
                    m_arr_iter = m_base->GetArray().GetVector().begin();
                }
            }
        }
        Iterator(const Iterator& other) {
            m_base = other.m_base;
        }

        ~Iterator() { }

        bool operator!=(const Iterator& other) const {
            if (m_base->GetType() == value::ValueType::kArray) {
                return m_base->GetObject().GetMap() == other.m_base->GetObject().GetMap();
            }
            else if (m_base->GetType() == value::ValueType::kObject) {
                return m_base->GetArray().GetVector() == other.m_base->GetArray().GetVector();
            }
            return false;
        }

        Json& operator*() const {
            if (m_base->GetType() == value::ValueType::kArray) {
                return (Json&)m_obj_iter->second;
            }
            else if (m_base->GetType() == value::ValueType::kObject) {
                return (Json&)*m_arr_iter;
            }
            return *(Json*)nullptr;
        }

        const Iterator& operator++() {
            if (m_base->GetType() == value::ValueType::kArray) {
                m_arr_iter++;
            }
            else if (m_base->GetType() == value::ValueType::kObject) {
                m_obj_iter++;
            }
            return *this;
        }
    private:
        union {
            value::ValuePtrtMap::const_iterator m_obj_iter;
            value::ValuePtrVector::const_iterator m_arr_iter;
        };
        Json* m_base;
    };

public:
    Iterator begin() {
        return Iterator(this);
    }

    Iterator end() {
        return Iterator(nullptr);
    }

private:
    void StrEscapr(_SCN string* str) const {
        // 引号添加\转义，一个\修改为两个\\ 
        *str = std::regex_replace(*str, std::regex(R"(\\)"), R"(\\)");
        *str = std::regex_replace(*str, std::regex(R"(")"), R"(\")");
    }

    void Print(value::ValueBase* value, bool format, size_t level, _SCN string* jsonStr) const {
        _SCN string indent;
        if (format) {
            indent = _SCN string(level * kIndent, '  ');
        }

        switch (value->Type()) {
        case value::ValueType::kNull: {
            *jsonStr += "null";
            break;
        }
        case value::ValueType::kBoolean: {
            *jsonStr += static_cast<value::BooleanValue*>(value)->Get() ? "true" : "false";
            break;
        }
        case value::ValueType::kNumber: {
            char buf[21]{ 0 };
            _itoa_s(static_cast<value::NumberValue*>(value)->GetInt(), buf, 10);
            *jsonStr += buf;
            break;
        }
        case value::ValueType::kString: {
            auto str = static_cast<value::StringValue*>(value)->Get();
            StrEscapr(&str);
            *jsonStr += "\"" + str + "\"";
            break;
        }
        case value::ValueType::kArray: {
            *jsonStr += '[';
            if (format) {
                indent += _SCN string(kIndent, '  ');
            }
            const auto& arr = static_cast<value::ArrayValue*>(value)->GetVector();
            for (int i = 0; i < arr.size(); ) {
                if (format) {
                    *jsonStr += '\n' + indent;
                }
                Print(arr[i].get(), format, level + 1, jsonStr);
                if (++i < arr.size()) {
                    *jsonStr += ", ";
                }
            }
            if (format) {
                for (int i = 0; i < kIndent; i++) {
                    indent.pop_back();
                }
                *jsonStr += '\n' + indent;
            }
            *jsonStr += ']';
            break;
        }
        case value::ValueType::kObject: {
            *jsonStr += '{';
            if (format) {
                indent += _SCN string(kIndent, '  ');
            }

            const auto& obj = static_cast<value::ObjectValue*>(value)->GetMap();
            int i = 0;
            for (const auto& it : obj) {
                if (format) {
                    *jsonStr += '\n' + indent;
                }
                auto key = it.first;
                StrEscapr(&key);
                *jsonStr += '\"' + key + "\":";

                Print(it.second.get(), format, level + 1, jsonStr);
                if (++i < obj.size()) {
                    *jsonStr += ", ";
                }

            }
            if (format) {
                for (int i = 0; i < 4; i++) {
                    indent.pop_back();
                }
                *jsonStr += '\n' + indent;
            }
            *jsonStr += '}';
            break;
        }
        }
    }

private:
    value::ValuePtr m_value;

private:
    static const int kIndent = 4;
};

} // namespace yubasic_Json

#endif // YUJSON_JSON_H_