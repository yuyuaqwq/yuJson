#ifndef YUJSON_JSON_HPP_
#define YUJSON_JSON_HPP_

// #define YUJSON_DISABLE_FLOAT

#include <string>
#include <memory>
#include <initializer_list>

#include <yuJson/compiler/parser.hpp>
#include <yuJson/value/value.hpp>

namespace yuJson {
class Json : private value::ValuePtr {
public:
    friend Json Object(std::initializer_list<Json> json_list);
    friend Json Array(std::initializer_list<Json> json_list);

public:
    class Iterator {
    public:
        friend class Json;
    public:
        Iterator(Json* base) {
            base_ = base;
            if (base) {
                if (base_->IsArray()) {
                    new(&arr_iter_) decltype(arr_iter_);
                    arr_iter_ = (*base_)->ToArray().GetVector().begin();
                }
                else if (base_->IsObject()) {
                    new(&obj_iter_) decltype(obj_iter_);
                    obj_iter_ = (*base_)->ToObject().GetMap().begin();
                }
                else {
                    throw value::ValueTypeError("Non container types cannot iterate");
                }
            }
        }
        Iterator(Json* base, const value::ValuePtrtMap::iterator& obj_iter) {
            base_ = base;
            new(&obj_iter_) decltype(obj_iter_);
            obj_iter_ = obj_iter;
        }
        Iterator(Json* base, const value::ValuePtrVector::iterator& arr_iter) {
            base_ = base;
            new(&arr_iter_) decltype(arr_iter_);
            arr_iter_ = arr_iter;
        }
        Iterator(const Iterator& other) noexcept {
            operator=(other);
        }

        template<class T>
        void Destroy(T* ptr) {
            ptr->~T();
        }

        ~Iterator() {
            if (base_) {
                if (base_->IsObject()) {
                    Destroy(&obj_iter_);
                }
                else if (base_->IsArray()) {
                    Destroy(&arr_iter_);
                }
                else {
                    throw value::ValueTypeError("Non container types cannot iterate");
                }
            }
        }
        void operator=(const Iterator& other) {
            base_ = other.base_;
            if (other.base_->IsObject()) {
                new(&obj_iter_) decltype(obj_iter_);
                obj_iter_ = other.obj_iter_;
            }
            else if (base_->IsArray()) {
                new(&arr_iter_) decltype(arr_iter_);
                arr_iter_ = other.arr_iter_;
            }
            else {
                throw value::ValueTypeError("Non container types cannot iterate");
            }
        }

        bool operator!=(const Iterator& other) const {
            if (base_ == nullptr && other.base_ == nullptr) {
                return false;
            }
            if (other.base_ == nullptr || base_ == nullptr) {
                return true;
            }
            if (base_->IsArray()) {
                return (*base_)->ToArray().GetVector() == (*other.base_)->ToArray().GetVector();
            }
            else if (base_->IsObject()) {
                return (*base_)->ToObject().GetMap() == (*other.base_)->ToObject().GetMap();
            }
            throw value::ValueTypeError("Non container types cannot iterate");
        }

        Iterator& operator*() {
            return *this;
        }

        const Iterator& operator++() {
            if ((*base_)->Type() == value::ValueType::kArray) {
                arr_iter_++;
                if (arr_iter_ == (*base_)->ToArray().GetVector().end()) {
                    base_ = nullptr;
                }
            }
            else if ((*base_)->Type() == value::ValueType::kObject) {
                obj_iter_++;
                if (obj_iter_ == (*base_)->ToObject().GetMap().end()) {
                    base_ = nullptr;
                }
            }
            return *this;
        }

    public:
        const std::string& key() {
            if ((*base_)->Type() == value::ValueType::kObject) {
                return obj_iter_->first;
            }
            throw value::ValueTypeError("Not an object iterator");
        }

        Json& value() {
            if ((*base_)->Type() == value::ValueType::kArray) {
                return static_cast<Json&>(*arr_iter_);
            }
            else if ((*base_)->Type() == value::ValueType::kObject) {
                return static_cast<Json&>(obj_iter_->second);
            }
            throw value::ValueTypeError("Non container types cannot iterate");
        }

    private:
        union {
            value::ValuePtrtMap::iterator obj_iter_;
            value::ValuePtrVector::iterator arr_iter_;
        };
        Json* base_;
    };

public:
    Json() noexcept { }
    explicit Json(value::ValuePtr value) noexcept : value::ValuePtr{ std::move(value) } { }
    Json(Json&& json) noexcept : value::ValuePtr{ std::move(json) } { }
    Json(nullptr_t) : value::ValuePtr{ std::make_unique<value::NullValue>() } { }
    Json(bool b) : value::ValuePtr{ std::make_unique<value::BooleanValue>(b) } { }
    Json(int i) : value::ValuePtr{ std::make_unique<value::NumberIntValue>(long long(i)) } { }
    Json(unsigned int i) : value::ValuePtr{ std::make_unique<value::NumberIntValue>(unsigned long long(i)) } { }
#ifndef YUJSON_DISABLE_FLOAT
    Json(double d) : value::ValuePtr{ std::make_unique<value::NumberFloatValue>(d) } { }
#endif
    Json(const char* str) : value::ValuePtr{ std::make_unique<value::StringValue>(str) } { }
    Json(const unsigned char* str) : value::ValuePtr{ std::make_unique<value::StringValue>((char*)str) } { }
    Json(std::string str) : value::ValuePtr{ std::make_unique<value::StringValue>(str) } { }
    Json(std::initializer_list<Json> json_list) {
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
        return *static_cast<Json*>(&((*this)->ToObject()[str]));
    }

    Json& operator[](int index) {
        return *static_cast<Json*>(&((*this)->ToArray()[index]));
    }

    Json& at(const char* str) {
        return *static_cast<Json*>(&((*this)->ToObject().At(str)));
    }

    Json& at(int index) {
        return *static_cast<Json*>(&((*this)->ToArray().At(index)));
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
            return (*this)->ToNumberInt().Get() == other->ToNumberInt().Get();
#ifndef YUJSON_DISABLE_FLOAT
        case value::ValueType::kNumberFloat:
            return (*this)->ToNumberFloat().Get() == other->ToNumberFloat().Get();
#endif
        case value::ValueType::kString:
            return (*this)->ToString().Get() == other->ToString().Get();
        default:
            return false;
        }
    }

    Iterator begin() {
        return Iterator(this);
    }

    Iterator end() {
        return Iterator(nullptr);
    }

    Iterator find(const char* str) {
        auto map_iter = (*this)->ToObject().GetMap().find(str);
        if (map_iter == (*this)->ToObject().GetMap().end()) {
            return Iterator{ nullptr };
        }
        return Iterator{ this, map_iter };
    }

    size_t size() {
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

    std::string Print(bool format = true) const {
        if (!this->get()) {
            return "";
        }
        std::string jsonStr;
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
        return (*this)->ToNumberInt().Get();
    }
#ifndef YUJSON_DISABLE_FLOAT
    double& Float() {
        return (*this)->ToNumberFloat().Get();
    }
#endif

    bool IsString() {
        if (!IsValid()) return false;
        return (*this)->IsString();
    }

    std::string& String() {
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
            return (*this)->GetNumberInt().Get();
        }
#ifndef YUJSON_DISABLE_FLOAT
        case value::ValueType::kNumberFloat:
            return (*this)->ToNumberFloat().Get();
#endif                   
        case value::ValueType::kString: {
            auto& str = (*this)->GetString().Get();
            if (str.empty() || str[0] < '0' && str[0] > '9') return 0;
            return std::stoll(str);
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
            return (*this)->GetNumberInt().Get();
        }
        case value::ValueType::kNumberFloat:
            return (*this)->ToNumberFloat().Get();             
        case value::ValueType::kString: {
            auto& str = (*this)->GetString().Get();
            if (str.empty() || str[0] < '0' && str[0] > '9') return 0;
            return std::stod(str);
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
            return std::to_string((*this)->GetNumberInt().Get());
        }
#ifndef YUJSON_DISABLE_FLOAT
        case value::ValueType::kNumberFloat:
            return std::to_string((*this)->ToNumberFloat().Get());
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
        return *static_cast<T*>(this->get());
    }

private:
    static std::string Replace(const std::string& str, const std::string& replace, const std::string& target) {
        auto new_str = str;
        size_t pos = 0;
        pos = new_str.find(replace);
        while (pos != -1) {
            new_str = new_str.replace(pos, replace.size(), target.c_str());
            pos = new_str.find(replace, pos + target.size());
        }
        return new_str;
    }

    void StrEscapr(std::string* str) const {
        // 引号添加\转义，一个\修改为两个\\ 
        *str = Replace(*str, R"(\)", R"(\\)");
        *str = Replace(*str, R"(")", R"(\")");
        *str = Replace(*str, R"(/)", R"(\/)");
        *str = Replace(*str, "\b", R"(\b)");
        *str = Replace(*str, "\r", R"(\r)");
        *str = Replace(*str, "\n", R"(\n)");
        *str = Replace(*str, "\t", R"(\t)");
    }

    void Print(value::ValueInterface* value, bool format, size_t level, std::string* jsonStr) const {
        std::string indent;
        if (format) {
            indent = std::string(level * kIndent, '    ');
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
            auto str = std::to_string(static_cast<value::NumberIntValue*>(value)->Get());
            *jsonStr += str;
            break;
        }
#ifndef YUJSON_DISABLE_FLOAT
        case value::ValueType::kNumberFloat: {
            auto str = std::to_string(static_cast<value::NumberFloatValue*>(value)->Get());
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
                indent += std::string(kIndent, '    ');
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
                indent += std::string(kIndent, '    ');
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

static inline Json Parse(const std::string& json_text) {
    compiler::Lexer lexer(json_text);
    compiler::Parser parser(&lexer);
    return Json(parser.ParseValue());
}

static inline Json Object(std::initializer_list<Json> json_list) {
    Json json{ std::make_unique<value::ObjectValue>() };
    for (auto iter = json_list.begin(); iter != json_list.end(); iter++, iter++) {
        std::string key = (*iter)->ToString().Get();
        json->ToObject().Set(key, std::move(static_cast<value::ValuePtr&>(*(const_cast<Json*>(iter) + 1))));
    }
    return json;
}
static inline Json Array(std::initializer_list<Json> json_list) {
    Json json{ std::make_unique<value::ArrayValue>() };
    for (auto iter = json_list.begin(); iter != json_list.end(); iter++) {
        json->ToArray().Pushback(std::move(static_cast<value::ValuePtr&>(const_cast<Json&>(*iter))));
    }
    return json;
}

} // namespace yuJson

#endif // YUJSON_JSON_HPP_