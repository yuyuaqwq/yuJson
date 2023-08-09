#ifndef YUJSON_JSON_H_
#define YUJSON_JSON_H_

#ifndef YUJSON_STD
#define YUJSON_STD std::
#endif // YUJSON_STD

#include <string>
#include <memory>
#include <initializer_list>

#include <yuJson/compiler/parser.hpp>

namespace yuJson {
class Json {
private:
    enum class ContainerType {
        kArray,
        kObject
    };

public:
    Json() noexcept { }
    explicit Json(value::ValuePtr value) noexcept : m_value{ std::move(value) } { }
    Json(Json&& json) noexcept : m_value{ std::move(json.m_value) } { }
    Json(nullptr_t) : m_value{ YUJSON_STD make_unique<value::NullValue>() } { }
    Json(bool b) : m_value{ YUJSON_STD make_unique<value::BooleanValue>(b) } { }
    Json(int i) : m_value{ YUJSON_STD make_unique<value::NumberValue>(long long(i)) } { }
    Json(unsigned int i) : m_value{ YUJSON_STD make_unique<value::NumberValue>(unsigned long long(i)) } { }
    Json(double d) : m_value{ YUJSON_STD make_unique<value::NumberValue>(d) } { }
    Json(const char* str) : m_value{ YUJSON_STD make_unique<value::StringValue>(str) } { }
    Json(YUJSON_STD string str) : m_value{ YUJSON_STD make_unique<value::StringValue>(str) } { }
#ifdef WINNT
    Json(YUJSON_STD list<Json>& jsons) {
#else
    Json(std::initializer_list<Json> jsons) {
#endif // WINNT
        if (jsons.size() % 2 == 0) {
            bool is_obj = true;
            for (auto iter = jsons.begin(); iter != jsons.end(); iter++, iter++) {
                if (!iter->m_value->IsString()) {
                    is_obj = false;
                    break;
                }
            }
            if (is_obj) {
                m_value = YUJSON_STD make_unique<value::ObjectValue>();
                for (auto iter = jsons.begin(); iter != jsons.end(); iter++, iter++) {
                    YUJSON_STD string key = iter->m_value->ToString().Get();
#ifdef WINNT
                    m_value->ToObject().Set(key, std::move((&*iter + 1)->m_value));
#else
                    m_value->ToObject().Set(key, std::move(((Json*)iter + 1)->m_value));
#endif // WINNT
                }
            }
        }
        if (!IsValid()) {
            m_value = YUJSON_STD make_unique<value::ArrayValue>();
            for (auto iter = jsons.begin(); iter != jsons.end(); iter++) {
#ifdef WINNT
                    m_value->ToArray().Pushback(std::move((&*iter)->m_value));
#else
                    m_value->ToArray().Pushback(std::move(((Json*)iter)->m_value));
#endif // WINNT
            }
        }
    }

private:
    Json(ContainerType type) {
        switch (type) {
        case ContainerType::kArray:
            m_value = YUJSON_STD make_unique<value::ArrayValue>();
            break;
        case ContainerType::kObject:
            m_value = YUJSON_STD make_unique<value::ObjectValue>();
            break;
        }
    }
public:
    static Json MakeObject() {
        return Json(ContainerType::kObject);
    }
    static Json MakeArray() {
        return Json(ContainerType::kArray);
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
        case value::ValueType::kNumber:
            return m_value->ToNumber().GetInt() == other.m_value->ToNumber().GetInt();
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
        return m_value->IsNumber();
    }

    long long& Int() {
        return m_value->ToNumber().GetInt();
    }

    double& Float() {
        return m_value->ToNumber().GetFloat();
    }

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
        case value::ValueType::kNumber: {
            return m_value->GetNumber().GetInt();
        }
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
        case value::ValueType::kNumber: {
            return YUJSON_STD to_string(m_value->GetNumber().GetInt());
        }
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
        Iterator(Json* base){
            m_base = base;
            if (base) {
                if (m_base->IsArray()) {
                    m_obj_iter = m_base->m_value->ToObject().GetMap().begin();
                }
                else if (m_base->IsObject()) {
                    m_arr_iter = m_base->m_value->ToArray().GetVector().begin();
                }
            }
        }
        Iterator(const Iterator& other) {
            m_base = other.m_base;
        }

        ~Iterator() { }

        bool operator!=(const Iterator& other) const {
            if (m_base->IsArray()) {
                return m_base->m_value->ToObject().GetMap() == other.m_base->m_value->ToObject().GetMap();
            }
            else if (m_base->IsObject()) {
                return m_base->m_value->ToArray().GetVector() == other.m_base->m_value->ToArray().GetVector();
            }
            return false;
        }

        Json& operator*() const {
            if (m_base->IsArray()) {
                return (Json&)m_obj_iter->second;
            }
            else if (m_base->IsObject()) {
                return (Json&)*m_arr_iter;
            }
            return *(Json*)nullptr;
        }

        const Iterator& operator++() {
            if (m_base->m_value->Type() == value::ValueType::kArray) {
                m_arr_iter++;
            }
            else if (m_base->m_value->Type() == value::ValueType::kObject) {
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
        case value::ValueType::kNumber: {
            //char str[64];
            //sprintf(str, "%lld", );
            auto str = YUJSON_STD to_string(static_cast<value::NumberValue*>(value)->GetInt());
            *jsonStr += str;
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