#ifndef YUJSON_JSON_H_
#define YUJSON_JSON_H_

#include <string>
#include <memory>
#include <regex>
#include <initializer_list>

#include <yuJson/compiler/parser.hpp>

namespace yuJson {
class Json {
private:
    //using json_string = std::basic_string<char, std::char_traits<char>, allocatorT<char>>;

public:
    Json() noexcept { }
    explicit Json(std::unique_ptr<value::Value> value) noexcept : m_value{ std::move(value) } { }
    Json(Json&& json) noexcept : m_value{ std::move(json.m_value) } { }
    Json(nullptr_t) : m_value{ std::make_unique<value::Null>() } { }
    Json(bool b) : m_value{ std::make_unique<value::Boolean>(b) } { }
    Json(int i) : m_value{ std::make_unique<value::Number>(long long(i)) } { }
    Json(unsigned int i) : m_value{ std::make_unique<value::Number>(unsigned long long(i)) } { }
    Json(double d) : m_value{ std::make_unique<value::Number>(d) } { }
    Json(const char* str) : m_value{ std::make_unique<value::String>(str) } { }
    Json(std::initializer_list<Json> json) {
        if (json.size() % 2 == 0) {
            bool is_obj = true;
            for (auto iter = json.begin(); iter != json.end(); iter+=2) {
                if (!iter->m_value->IsString()) {
                    is_obj = false;
                    break;
                }
            }
            if (is_obj) {
                m_value = std::make_unique<value::Object>();
                for (auto iter = json.begin(); iter != json.end(); iter += 2) {
                    std::string key = iter->m_value->ToString().Get();
                    m_value->ToObject().Set(key, std::move(((Json*)iter + 1)->m_value));
                }
            }
        }
        if (!IsValid()) {
            m_value = std::make_unique<value::Array>();
            for (auto iter = json.begin(); iter != json.end(); iter++) {
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
        auto tempJson = std::make_unique<Json>(parser.ParseValue());
        if (!tempJson->IsValid()) {
            return false;
        }
        m_value = std::move(tempJson->m_value);
        return true;
    }

    std::string Print(bool format = true) const {
        if (!m_value.get()) {
            return "";
        }
        std::string jsonStr;
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

    value::Null& GetNull() {
        return m_value->ToNull();
    }

    value::Boolean& GetBoolean() {
        return m_value->ToBoolean();
    }

    bool& Boolean() {
        return m_value->ToBoolean().Get();
    }

    value::Number& GetNumber() {
        return m_value->ToNumber();
    }

    long long& Int() {
        return m_value->ToNumber().GetInt();
    }

    double& Float() {
        return m_value->ToNumber().GetFloat();
    }

    value::String& GetString() {
        return m_value->ToString();
    }

    std::string& String() {
        return m_value->ToString().Get();
    }

    value::Array& GetArray() {
        return m_value->ToArray();
    }

    value::Object& GetObject() {
        return m_value->ToObject();
    }

    void Set(std::unique_ptr<value::Value> value) {
        m_value = std::move(value);
    }

    //template <typename T>
    //void Set(T&& val) {
    //    m_value = std::make_unique<T>(std::move(val));
    //}

    void Set(nullptr_t) {
        m_value = std::make_unique<value::Null>();
    }

    void Set(value::Boolean&& boolean) {
        m_value = std::make_unique<value::Boolean>(std::move(boolean));
    }

    void Set(value::Number&& num) {
        m_value = std::make_unique<value::Number>(std::move(num));
    }

    void Set(value::String&& str) {
        m_value = std::make_unique<value::String>(std::move(str));
    }

    void Set(value::Array&& arr) {
        m_value = std::make_unique<value::Array>(std::move(arr));
    }

    void Set(value::Object&& obj) {
        m_value = std::make_unique<value::Object>(std::move(obj));
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
            std::map<std::string, std::unique_ptr<value::Value>>::const_iterator m_obj_iter;
            std::vector<std::unique_ptr<value::Value>>::const_iterator m_arr_iter;
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
    void StrEscapr(std::string* str) const {
        // 引号添加\转义，一个\修改为两个\\ 
        *str = std::regex_replace(*str, std::regex(R"(\\)"), R"(\\)");
        *str = std::regex_replace(*str, std::regex(R"(")"), R"(\")");
    }

    void Print(value::Value* value, bool format, size_t level, std::string* jsonStr) const {
        std::string indent;
        if (format) {
            indent = std::string(level * kIndent, '  ');
        }

        switch (value->Type()) {
        case value::ValueType::kNull: {
            *jsonStr += "null";
            break;
        }
        case value::ValueType::kBoolean: {
            *jsonStr += static_cast<value::Boolean*>(value)->Get() ? "true" : "false";
            break;
        }
        case value::ValueType::kNumber: {
            *jsonStr += std::to_string(static_cast<value::Number*>(value)->GetInt());
            break;
        }
        case value::ValueType::kString: {
            auto str = static_cast<value::String*>(value)->Get();
            StrEscapr(&str);
            *jsonStr += "\"" + str + "\"";
            break;
        }
        case value::ValueType::kArray: {
            *jsonStr += '[';
            if (format) {
                indent += std::string(kIndent, '  ');
            }
            const auto& arr = static_cast<value::Array*>(value)->GetVector();
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
                
                indent += std::string(kIndent, '  ');
            }

            const auto& obj = static_cast<value::Object*>(value)->GetMap();
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
    std::unique_ptr<value::Value> m_value;

private:
    static const int kIndent = 4;
};

//using Json = Json<std::allocator>;

} // namespace yuJson

#endif // YUJSON_JSON_H_