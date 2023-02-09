#ifndef YUJSON_JSON_H_
#define YUJSON_JSON_H_

#include <string>
#include <memory>
#include <regex>

#include <yuJson/compiler/parser.hpp>
#include <yuJson/value/value.hpp>

namespace yuJson {

class Json {
public:
    Json() noexcept { }
    explicit Json(std::unique_ptr<value::Value> value) noexcept : m_value{ std::move(value) } { }
    Json(Json&& json) noexcept : m_value{ std::move(json.m_value) } { }

    ~Json() noexcept { }

    Json(const Json&) = delete;
    void operator=(const Json&) = delete;

    bool IsValid() noexcept {
        return m_value.get() != nullptr;
    }

    bool Parse(const char* jsonText) {
        compiler::Lexer lexer(jsonText);
        compiler::Parser parser(&lexer);
        auto tempJson = parser.ParseJson();
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

    value::Value& Get() noexcept {
        return *m_value;
    }

    template <typename T>
    T& Get() noexcept {
        return *(T*)m_value.get();
    }

    value::Null& GetNull() noexcept {
        return m_value->ToNull();
    }

    value::Boolean& GetBoolean() noexcept {
        return m_value->ToBoolean();
    }

    value::Number& GetNumber() noexcept {
        return m_value->ToNumber();
    }

    value::String& GetString() noexcept {
        return m_value->ToString();
    }

    value::Array& GetArray() noexcept {
        return m_value->ToArray();
    }

    value::Object& GetObject() noexcept {
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

private:
    void StrEscapr(std::string* str) const {
        // 只处理引号的转义
        // *str = std::regex_replace(*str, std::regex(R"(\\)"), R"(\\")");
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

} // namespace yuJson

#endif // YUJSON_JSON_H_