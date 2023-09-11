#ifndef YUJSON_JSON_HPP_
#define YUJSON_JSON_HPP_

#ifndef YUJSON_STD
#define YUJSON_STD std::
#endif // YUJSON_STD

// #define YUJSON_DISABLE_FLOAT

#include <string>
#include <memory>
#include <initializer_list>

#include <yuJson/compiler/parser.hpp>
#include <yuJson/value/value.hpp>

namespace yuJson {
class Json : private value::ValuePtr {
public:
#ifdef WINNT
    friend Json Object(YUJSON_STD list<Json>& json_list) {
    friend Json Array(YUJSON_STD list<Json>&json_list) {
#else
    friend Json Object(YUJSON_STD initializer_list<Json> json_list);
    friend Json Array(YUJSON_STD initializer_list<Json> json_list);
#endif

public:
    class Iterator {
    public:
        Iterator(Json* base) {
            m_base = base;
            if (base) {
                if (m_base->IsArray()) {
                    m_arr_iter = (*m_base)->ToArray().GetVector().begin();
                }
                else if (m_base->IsObject()) {
                    m_obj_iter = (*m_base)->ToObject().GetMap().begin();
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
                return (*m_base)->ToArray().GetVector() == (*other.m_base)->ToArray().GetVector();
            }
            else if (m_base->IsObject()) {
                return (*m_base)->ToObject().GetMap() == (*other.m_base)->ToObject().GetMap();
            }
            throw value::ValueTypeError("Non container types cannot iterate");
        }

        Iterator& operator*() {
            return *this;
        }

        const Iterator& operator++() {
            if ((*m_base)->Type() == value::ValueType::kArray) {
                m_arr_iter++;
                if (m_arr_iter == (*m_base)->ToArray().GetVector().end()) {
                    m_base = nullptr;
                }
            }
            else if ((*m_base)->Type() == value::ValueType::kObject) {
                m_obj_iter++;
                if (m_obj_iter == (*m_base)->ToObject().GetMap().end()) {
                    m_base = nullptr;
                }
            }
            return *this;
        }

    public:
        const YUJSON_STD string& key() {
            if ((*m_base)->Type() == value::ValueType::kObject) {
                return m_obj_iter->first;
            }
            throw value::ValueTypeError("Not an object iterator");
        }

        Json& value() {
            if ((*m_base)->Type() == value::ValueType::kArray) {
                return (Json&)*m_arr_iter;
            }
            else if ((*m_base)->Type() == value::ValueType::kObject) {
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

public:
    Json() noexcept { }
    explicit Json(value::ValuePtr&& value) noexcept : value::ValuePtr{ std::move(value) } { }
    Json(Json&& json) noexcept : value::ValuePtr{ std::move(json) } { }
    Json(nullptr_t) : value::ValuePtr{ YUJSON_STD make_unique<value::NullValue>() } { }
    Json(bool b) : value::ValuePtr{ YUJSON_STD make_unique<value::BooleanValue>(b) } { }
    Json(int i) : value::ValuePtr{ YUJSON_STD make_unique<value::NumberIntValue>(long long(i)) } { }
    Json(unsigned int i) : value::ValuePtr{ YUJSON_STD make_unique<value::NumberIntValue>(unsigned long long(i)) } { }
#ifndef YUJSON_DISABLE_FLOAT
    Json(double d) : value::ValuePtr{ YUJSON_STD make_unique<value::NumberFloatValue>(d) } { }
#endif
    Json(const char* str) : value::ValuePtr{ YUJSON_STD make_unique<value::StringValue>(str) } { }
    Json(const unsigned char* str) : value::ValuePtr{ YUJSON_STD make_unique<value::StringValue>((char*)str) } { }
    Json(YUJSON_STD string str) : value::ValuePtr{ YUJSON_STD make_unique<value::StringValue>(str) } { }
#ifdef WINNT
    Json(YUJSON_STD list<Json>& json_list) {
#else
    Json(std::initializer_list<Json> json_list) {
#endif // WINNT
        if (json_list.size() % 2 == 0) {
            bool is_obj = true;
            for (auto iter = json_list.begin(); iter != json_list.end(); iter++, iter++) {
                if (!(*iter)->IsString()) {
                    is_obj = false;
                    break;
                }
            }
            if (is_obj) {
                auto obj_json = Object(json_list);
                *this = std::move(obj_json);
            }
        }
        if (!IsValid()) {
            auto arr_json = Array(json_list);
            *this = std::move(arr_json);
        }
    }

    ~Json() noexcept { }

    Json(const Json&) = delete;
    void operator=(const Json&) = delete;

    void operator=(Json&& json) noexcept {
        this->reset(json.release());
    }

    Json& operator[](const char* str) {
        return *(Json*)&((*this)->ToObject()[str]);
    }

    Json& operator[](int index) {
        return *(Json*)&((*this)->ToArray()[index]);
    }

    Json& At(const char* str) {
        return *(Json*)&((*this)->ToObject().At(str));
    }

    Json& At(int index) {
        return *(Json*)&((*this)->ToArray().At(index));
    }

    bool operator==(const Json&& other) const {
        if ((*this)->Type() != other->Type()) {
            return false;
        }
        switch ((*this)->Type()) {
        case value::ValueType::kNull:
            return true;
        case value::ValueType::kBoolean:
            return (*this)->ToBoolean().Get() == other->ToBoolean().Get();
        case value::ValueType::kNumberInt:
            return (*this)->ToNumberInt().GetInt() == other->ToNumberInt().GetInt();
#ifndef YUJSON_DISABLE_FLOAT
        case value::ValueType::kNumberFloat:
            return (*this)->ToNumberFloat().GetFloat() == other->ToNumberFloat().GetFloat();
#endif
        case value::ValueType::kString:
            return (*this)->ToString().Get() == other->ToString().Get();
        default:
            return false;
        }
    }

    bool Find(const char* str) {
        return (*this)->ToObject().Find(str);
    }

    size_t Size() {
        if (IsArray()) {
            return (*this)->ToArray().GetVector().size();
        }
        else if (IsObject()) {
            return (*this)->ToObject().GetMap().size();
        }
        else if (IsString()) {
            return String().size();
        }
        else {
            throw value::ValueTypeError("Unable to view the type of size");
        }
    }

    bool IsValid() noexcept {
        return this->get() != nullptr;
    }

    YUJSON_STD string Print(bool format = true) const {
        if (!this->get()) {
            return "";
        }
        YUJSON_STD string jsonStr;
        Print(this->get(), format, 0, &jsonStr);
        return jsonStr;
    }

    bool IsNull() {
        if (!IsValid()) return false;
        return (*this)->IsNull();
    }

    bool IsBoolean() {
        if (!IsValid()) return false;
        return (*this)->IsBoolean();
    }

    bool& Boolean() {
        return (*this)->ToBoolean().Get();
    }

    bool IsNumber() {
        if (!IsValid()) return false;
        return (*this)->IsNumberInt() || (*this)->IsNumberFloat();
    }

    long long& Int() {
        return (*this)->ToNumberInt().GetInt();
    }
#ifndef YUJSON_DISABLE_FLOAT
    double& Float() {
        return (*this)->ToNumberFloat().GetFloat();
    }
#endif

    bool IsString() {
        if (!IsValid()) return false;
        return (*this)->IsString();
    }

    YUJSON_STD string& String() {
        return (*this)->ToString().Get();
    }

    bool IsArray() {
        if (!IsValid()) return false;
        return (*this)->IsArray();
    }

    bool IsObject() {
        if (!IsValid()) return false;
        return (*this)->IsObject();
    }


    long long ConvertToInt(long long defalut = 0) {
        if (!IsValid()) return defalut;
        switch ((*this)->Type()) {
        case value::ValueType::kNumberInt: {
            return (*this)->GetNumberInt().GetInt();
        }
#ifndef YUJSON_DISABLE_FLOAT
        case value::ValueType::kNumberFloat:
            return (*this)->ToNumberFloat().GetFloat();
#endif                   
        case value::ValueType::kString: {
            auto& str = (*this)->GetString().Get();
            if (str.empty() || str[0] < '0' && str[0] > '9') return 0;
            return YUJSON_STD stoll(str);
        }
        case value::ValueType::kNull: {
            return defalut;
        }
        case value::ValueType::kBoolean: {
            return (*this)->GetBoolean().Get() ? 1 : 0;
        }
        defalut: {
            throw value::ValueTypeError("Object and Array cannot be converted to Int");
        }
        }
    }

#ifndef YUJSON_DISABLE_FLOAT
    double ConvertToFloat(double defalut = 0.0) {
        if (!IsValid()) return defalut;
        switch ((*this)->Type()) {
        case value::ValueType::kNumberInt: {
            return (*this)->GetNumberInt().GetInt();
        }
        case value::ValueType::kNumberFloat:
            return (*this)->ToNumberFloat().GetFloat();             
        case value::ValueType::kString: {
            auto& str = (*this)->GetString().Get();
            if (str.empty() || str[0] < '0' && str[0] > '9') return 0;
            return YUJSON_STD stod(str);
        }
        case value::ValueType::kNull: {
            return defalut;
        }
        defalut: {
            throw value::ValueTypeError("Object and Array cannot be converted to Float");
        }
        }
    }
#endif

    std::string ConvertToString(std::string defalut = "") {
        if (!IsValid()) return defalut;
        switch ((*this)->Type()) {
        case value::ValueType::kNumberInt: {
            return YUJSON_STD to_string((*this)->GetNumberInt().GetInt());
        }
#ifndef YUJSON_DISABLE_FLOAT
        case value::ValueType::kNumberFloat:
            return YUJSON_STD to_string((*this)->ToNumberFloat().GetFloat());
#endif    
        case value::ValueType::kString: {
            return (*this)->GetString().Get().c_str();
        }
        case value::ValueType::kNull: {
            return defalut;
        }
        case value::ValueType::kBoolean: {
            return (*this)->GetBoolean().Get() ? "true" : "false";
        }
        defalut: {
            throw value::ValueTypeError("Object and Array cannot be converted to String");
        }
        }
    }

private:
    template <typename T>
    T& Get() noexcept {
        return *(T*)this->get();
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
        *str = Replace(*str, R"(/)", R"(\/)");
        *str = Replace(*str, "\b", R"(\b)");
        *str = Replace(*str, "\r", R"(\r)");
        *str = Replace(*str, "\n", R"(\n)");
        *str = Replace(*str, "\t", R"(\t)");
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
#ifndef YUJSON_DISABLE_FLOAT
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
    static constexpr int kIndent = 4;
};

static inline Json Parse(const YUJSON_STD string& json_text) {
    compiler::Lexer lexer(json_text);
    compiler::Parser parser(&lexer);
    return Json(parser.ParseValue());
}

#ifdef WINNT
static inline Json Object(YUJSON_STD list<Json>& json_list) {
#else
static inline Json Object(YUJSON_STD initializer_list<Json> json_list) {
#endif
    Json json{ YUJSON_STD make_unique<value::ObjectValue>() };
    for (auto iter = json_list.begin(); iter != json_list.end(); iter++, iter++) {
        YUJSON_STD string key = (*iter)->ToString().Get();
#ifdef WINNT
        json->ToObject().Set(key, (value::ValuePtr&&)*(iter + 1));
#else
        json->ToObject().Set(key, (value::ValuePtr&&)*(iter + 1));
#endif // WINNT
    }
    return json;
}
#ifdef WINNT
static inline Json Array(YUJSON_STD list<Json>& json_list) {
#else
static inline Json Array(YUJSON_STD initializer_list<Json> json_list) {
#endif
    Json json{ YUJSON_STD make_unique<value::ArrayValue>() };
    for (auto iter = json_list.begin(); iter != json_list.end(); iter++) {
#ifdef WINNT
        json->ToArray().Pushback((value::ValuePtr&&)*iter);
#else
        json->ToArray().Pushback((value::ValuePtr&&)*iter);
#endif // WINNT
    }
    return json;
}

} // namespace yuJson

#endif // YUJSON_JSON_HPP_