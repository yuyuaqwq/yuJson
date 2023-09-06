#ifndef YUJSON_JSON_H_
#define YUJSON_JSON_H_

#ifndef YUJSON_STD
#define YUJSON_STD std::
#endif // YUJSON_STD

// #define YUJSON_ENABLE_FLOAT

#include <string>
#include <memory>
#include <initializer_list>

#include <yuJson/compiler/parser.hpp>
#include <yuJson/value/value.hpp>

namespace yuJson {
class Json {
public:
    Json() noexcept { }
    explicit Json(value::ValuePtr value) noexcept : m_value{ std::move(value) } { }
    Json(Json&& json) noexcept : m_value{ std::move(json.m_value) } { }
    Json(nullptr_t) : m_value{ YUJSON_STD make_unique<value::NullValue>() } { }
    Json(bool b) : m_value{ YUJSON_STD make_unique<value::BooleanValue>(b) } { }
    Json(int i) : m_value{ YUJSON_STD make_unique<value::NumberIntValue>(long long(i)) } { }
    Json(unsigned int i) : m_value{ YUJSON_STD make_unique<value::NumberIntValue>(unsigned long long(i)) } { }
#ifdef YUJSON_ENABLE_FLOAT
    Json(double d) : m_value{ YUJSON_STD make_unique<value::NumberFloatValue>(d) } { }
#endif
    Json(const char* str) : m_value{ YUJSON_STD make_unique<value::StringValue>(str) } { }
    Json(const unsigned char* str) : m_value{ YUJSON_STD make_unique<value::StringValue>((char*)str) } { }
    Json(YUJSON_STD string str) : m_value{ YUJSON_STD make_unique<value::StringValue>(str) } { }
#ifdef WINNT
    Json(YUJSON_STD list<Json>& jsons) {
#else
    Json(std::initializer_list<Json> json_list) {
#endif // WINNT
        if (json_list.size() % 2 == 0) {
            bool is_obj = true;
            for (auto iter = json_list.begin(); iter != json_list.end(); iter++, iter++) {
                if (!iter->m_value->IsString()) {
                    is_obj = false;
                    break;
                }
            }
            if (is_obj) {
                auto obj_json = Object(json_list);
                m_value = std::move(obj_json.m_value);
            }
        }
        if (!IsValid()) {
            auto arr_json = Array(json_list);
            m_value = std::move(arr_json.m_value);
        }
    }
public:
#ifdef WINNT
    static Json Object(YUJSON_STD list<Json>&jsons) {
#else
    static Json Object(std::initializer_list<Json> json_list) {
#endif
        Json json;
        json.m_value = YUJSON_STD make_unique<value::ObjectValue>();
        for (auto iter = json_list.begin(); iter != json_list.end(); iter++, iter++) {
            YUJSON_STD string key = iter->m_value->ToString().Get();
#ifdef WINNT
            json.m_value->ToObject().Set(key, std::move((&*iter + 1)->m_value));
#else
            json.m_value->ToObject().Set(key, std::move(((Json*)iter + 1)->m_value));
#endif // WINNT
        }
        return json;
    }
#ifdef WINNT
    static Json Object(YUJSON_STD list<Json>& jsons) {
#else
    static Json Array(std::initializer_list<Json> json_list) {
#endif
        Json json;
        json.m_value = YUJSON_STD make_unique<value::ArrayValue>();
        for (auto iter = json_list.begin(); iter != json_list.end(); iter++) {
#ifdef WINNT
            json.m_value->ToArray().Pushback(std::move((&*iter)->m_value));
#else
            json.m_value->ToArray().Pushback(std::move(((Json*)iter)->m_value));
#endif // WINNT
        }
        return json;
    }

    ~Json() noexcept { }

    Json(const Json&) = delete;
    void operator=(const Json&) = delete;

    void operator=(Json&& json) noexcept {
        m_value = std::move(json.m_value);
    }

    Json& operator[](const char* str) {
        return *(Json*)&(m_value->ToObject()[str]);
    }

    Json& operator[](int index) {
        return *(Json*)&(m_value->ToArray()[index]);
    }

    Json& At(const char* str) {
        return *(Json*)&(m_value->ToObject().At(str));
    }

    Json& At(int index) {
        return *(Json*)&(m_value->ToArray().At(index));
    }

    bool operator==(const Json&& other) const {
        if (m_value->Type() != other.m_value->Type()) {
            return false;
        }
        switch (m_value->Type()) {
        case value::ValueType::kNull:
            return true;
        case value::ValueType::kBoolean:
            return    m_value->ToBoolean().Get() == other.m_value->ToBoolean().Get();
        case value::ValueType::kNumberInt:
            return m_value->ToNumberInt().GetInt() == other.m_value->ToNumberInt().GetInt();
#ifdef YUJSON_ENABLE_FLOAT
        case value::ValueType::kNumberFloat:
            return m_value->ToNumberFloat().GetFloat() == other.m_value->ToNumberFloat().GetFloat();
#endif
        case value::ValueType::kString:
            return m_value->ToString().Get() == other.m_value->ToString().Get();
        default:
            return false;
        }
    }

    bool Find(const char* str) {
        return m_value->ToObject().Find(str);
    }

    bool IsValid() noexcept {
        return m_value.get() != nullptr;
    }

    static Json Parse(const char* jsonText) {
        compiler::Lexer lexer(jsonText);
        compiler::Parser parser(&lexer);
        return Json(parser.ParseValue());
    }

    YUJSON_STD string Print(bool format = true) const {
        if (!m_value.get()) {
            return "";
        }
        YUJSON_STD string jsonStr;
        Print(m_value.get(), format, 0, &jsonStr);
        return jsonStr;
    }

    bool IsNull() {
        if (!IsValid()) return false;
        return m_value->IsNull();
    }

    bool IsBoolean() {
        if (!IsValid()) return false;
        return m_value->IsBoolean();
    }

    bool& Boolean() {
        return m_value->ToBoolean().Get();
    }

    bool IsNumber() {
        if (!IsValid()) return false;
        return m_value->IsNumberInt() || m_value->IsNumberFloat();
    }

    long long& Int() {
        return m_value->ToNumberInt().GetInt();
    }
#ifdef YUJSON_ENABLE_FLOAT
    double& Float() {
        return m_value->ToNumberFloat().GetFloat();
    }
#endif

    bool IsString() {
        if (!IsValid()) return false;
        return m_value->IsString();
    }

    YUJSON_STD string& String() {
        return m_value->ToString().Get();
    }

    bool IsArray() {
        if (!IsValid()) return false;
        return m_value->IsArray();
    }

    bool IsObject() {
        if (!IsValid()) return false;
        return m_value->IsObject();
    }


    long long ConvertToInt(long long defalut = 0) {
        if (!IsValid()) return defalut;
        switch (m_value->Type()) {
        case value::ValueType::kNumberInt: {
            return m_value->GetNumberInt().GetInt();
        }
#ifdef YUJSON_ENABLE_FLOAT
        case value::ValueType::kNumberFloat:
            return m_value->ToNumberFloat().GetFloat();
#endif                   
        case value::ValueType::kString: {
            auto& str = m_value->GetString().Get();
            if (str.empty() || str[0] < '0' && str[0] > '9') return 0;
            return YUJSON_STD stoll(str);
        }
        case value::ValueType::kNull: {
            return defalut;
        }
        case value::ValueType::kBoolean: {
            return m_value->GetBoolean().Get() ? 1 : 0;
        }
        defalut: {
            throw value::ValueTypeError("Object and Array cannot be converted to Int");
        }
        }
    }

    std::string ConvertToString(std::string defalut = "") {
        if (!IsValid()) return defalut;
        switch (m_value->Type()) {
        case value::ValueType::kNumberInt: {
            return YUJSON_STD to_string(m_value->GetNumberInt().GetInt());
        }
#ifdef YUJSON_ENABLE_FLOAT
        case value::ValueType::kNumberFloat:
            return YUJSON_STD to_string(m_value->ToNumberFloat().GetFloat());
#endif    
        case value::ValueType::kString: {
            return m_value->GetString().Get().c_str();
        }
        case value::ValueType::kNull: {
            return defalut;
        }
        case value::ValueType::kBoolean: {
            return m_value->GetBoolean().Get() ? "true" : "false";
        }
        defalut: {
            throw value::ValueTypeError("Object and Array cannot be converted to String");
        }
        }
    }

private:
    template <typename T>
    T& Get() noexcept {
        return *(T*)m_value.get();
    }

public:
    class Iterator {
    public:
        Iterator(Json* base) {
            m_base = base;
            if (base) {
                if (m_base->IsArray()) {
                    m_arr_iter = m_base->m_value->ToArray().GetVector().begin();
                }
                else if (m_base->IsObject()) {
                    m_obj_iter = m_base->m_value->ToObject().GetMap().begin();
                }
                else {
                    throw value::ValueTypeError("Non container types cannot iterate");
                }
            }
        }
        Iterator(Iterator&& other) noexcept {
            m_obj_iter = std::move(other.m_obj_iter);
            m_arr_iter = std::move(other.m_arr_iter);
            m_base = std::move(other.m_base);
        }
        Iterator(const Iterator& other) = delete;
        void operator=(const Iterator& other) = delete;
        ~Iterator() { }

        bool operator!=(const Iterator& other) const {
            if (m_base == nullptr && other.m_base == nullptr) {
                return false;
            }
            if (other.m_base == nullptr || m_base == nullptr) {
                return true;
            }
            if (m_base->IsArray()) {
                return m_base->m_value->ToArray().GetVector() == other.m_base->m_value->ToArray().GetVector();
            }
            else if (m_base->IsObject()) {
                return m_base->m_value->ToObject().GetMap() == other.m_base->m_value->ToObject().GetMap();
            }
            throw value::ValueTypeError("Non container types cannot iterate");
        }

        Iterator& operator*() {
            return *this;
        }

        const Iterator& operator++() {
            if (m_base->m_value->Type() == value::ValueType::kArray) {
                m_arr_iter++;
                if (m_arr_iter == m_base->m_value->ToArray().GetVector().end()) {
                    m_base = nullptr;
                }
            }
            else if (m_base->m_value->Type() == value::ValueType::kObject) {
                m_obj_iter++;
                if (m_obj_iter == m_base->m_value->ToObject().GetMap().end()) {
                    m_base = nullptr;
                }
            }
            return *this;
        }


        const YUJSON_STD string& key() {
            if (m_base->m_value->Type() == value::ValueType::kObject) {
                return m_obj_iter->first;
            }
            throw value::ValueTypeError("Not an object iterator");
        }

        Json& value() {
            if (m_base->m_value->Type() == value::ValueType::kArray) {
                return (Json&)*m_arr_iter;
            }
            else if (m_base->m_value->Type() == value::ValueType::kObject) {
                return (Json&)m_obj_iter->second;
            }
            throw value::ValueTypeError("Non container types cannot iterate");
        }

    private:
        union {
            value::ValuePtrtMap::iterator m_obj_iter;
            value::ValuePtrVector::iterator m_arr_iter;
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
    static YUJSON_STD string Replace(const YUJSON_STD string& str, const YUJSON_STD string& replace, const YUJSON_STD string& target) {
        auto new_str = str;
        size_t pos = 0;
        pos = new_str.find(replace);
        while (pos != -1) {
            new_str = new_str.replace(pos, replace.size(), target.c_str());
            pos = new_str.find(replace, pos + target.size());
        }
        return new_str;
    }
    void StrEscapr(YUJSON_STD string* str) const {
        // 引号添加\转义，一个\修改为两个\\ 
        *str = Replace(*str, R"(\)", R"(\\)");
        *str = Replace(*str, R"(")", R"(\")");
    }

    void Print(value::ValueInterface* value, bool format, size_t level, YUJSON_STD string* jsonStr) const {
        YUJSON_STD string indent;
        if (format) {
            indent = YUJSON_STD string(level * kIndent, '    ');
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
        case value::ValueType::kNumberInt: {
            //char str[64];
            //sprintf(str, "%lld", );
            auto str = YUJSON_STD to_string(static_cast<value::NumberIntValue*>(value)->GetInt());
            *jsonStr += str;
            break;
        }
#ifdef YUJSON_ENABLE_FLOAT
        case value::ValueType::kNumberFloat: {
            auto str = YUJSON_STD to_string(static_cast<value::NumberFloatValue*>(value)->GetFloat());
            *jsonStr += str;
            break;
        }
#endif 
        case value::ValueType::kString: {
            auto str = static_cast<value::StringValue*>(value)->Get();
            StrEscapr(&str);
            *jsonStr += "\"" + str + "\"";
            break;
        }
        case value::ValueType::kArray: {
            *jsonStr += '[';
            if (format) {
                indent += YUJSON_STD string(kIndent, '    ');
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
                indent += YUJSON_STD string(kIndent, '    ');
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

} // namespace yuJson

#endif // YUJSON_JSON_H_