#ifndef YUJSON_JSON_HPP_
#define YUJSON_JSON_HPP_

// #define YUJSON_DISABLE_FLOAT

#include <string>
#include <regex>
#include <memory>
#include <initializer_list>

#include <yuJson/compiler/parser.hpp>
#include <yuJson/value/value.hpp>

namespace yuJson {
class Json : private value::ValuePtr {
public:
    using Base = value::ValuePtr;

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

        bool operator==(const Iterator& other) const {
            return !operator!=(other);
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
        const std::string& key() const {
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
    static Json Parse(const std::string& json_text) {
        compiler::Lexer lexer(json_text);
        compiler::Parser parser(&lexer);
        return Json(parser.ParseValue());
    }
    static Json Object(std::initializer_list<Json> json_list = {}) {
        Json json{ std::make_unique<value::ObjectValue>() };
        for (auto iter = json_list.begin(); iter != json_list.end(); iter++, iter++) {
            std::string key = (*iter)->ToString().Get();
            json->ToObject().Set(key, std::move(static_cast<value::ValuePtr&>(*(const_cast<Json*>(iter) + 1))));
        }
        return json;
    }
    static Json Array(std::initializer_list<Json> json_list = {}) {
        Json json{ std::make_unique<value::ArrayValue>() };
        for (auto iter = json_list.begin(); iter != json_list.end(); iter++) {
            json->ToArray().PushBack(std::move(static_cast<value::ValuePtr&>(const_cast<Json&>(*iter))));
        }
        return json;
    }

public:
    Json() noexcept { }
    explicit Json(value::ValuePtr value) noexcept : value::ValuePtr{ std::move(value) } { }
    Json(Json&& json) noexcept : value::ValuePtr{ std::move(json) } { }
    Json(nullptr_t) : value::ValuePtr{ std::make_unique<value::NullValue>() } { }
    Json(bool b) : value::ValuePtr{ std::make_unique<value::BooleanValue>(b) } { }
    Json(int i) : value::ValuePtr{ std::make_unique<value::NumberIntValue>(int64_t(i)) } { }
    Json(unsigned int i) : value::ValuePtr{ std::make_unique<value::NumberIntValue>(uint64_t(i)) } { }
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
        return static_cast<Json&>(GetValue().ToObject()[str]);
    }

    Json& operator[](int index) {
        return static_cast<Json&>(GetValue().ToArray()[index]);
    }

    Json& at(const char* key) {
        return static_cast<Json&>(GetValue().ToObject().At(key));
    }

    Json& at(int index) {
        return static_cast<Json&>(GetValue().ToArray().At(index));
    }

    void push_back(Json&& val) {
        GetValue().ToArray().PushBack(std::move(val));
    }

    bool operator==(const Json& other) const {
        if (GetValue().Type() != other->Type()) {
            return false;
        }
        switch (GetValue().Type()) {
        case value::ValueType::kNull:
            return true;
        case value::ValueType::kBoolean:
            return GetValue().ToBoolean().Get() == other->ToBoolean().Get();
        case value::ValueType::kNumberInt:
            return GetValue().ToNumberInt().Get() == other->ToNumberInt().Get();
#ifndef YUJSON_DISABLE_FLOAT
        case value::ValueType::kNumberFloat:
            return GetValue().ToNumberFloat().Get() == other->ToNumberFloat().Get();
#endif
        case value::ValueType::kString:
            return GetValue().ToString().Get() == other->ToString().Get();
        default:
            return false;
        }
    }

    Iterator begin() {
        return Iterator{ this };
    }

    Iterator end() {
        return Iterator{ nullptr };
    }

    Iterator find(const char* key) {
        auto map_iter = GetValue().ToObject().GetMap().find(key);
        if (map_iter == GetValue().ToObject().GetMap().end()) {
            return Iterator{ nullptr };
        }
        return Iterator{ this, map_iter };
    }

    size_t size() const {
        if (IsArray()) {
            return GetValue().ToArray().GetVector().size();
        }
        else if (IsObject()) {
            return GetValue().ToObject().GetMap().size();
        }
        else if (IsString()) {
            return String().size();
        }
        else {
            throw value::ValueTypeError("Unable to view the type of size");
        }
    }

    bool erase(const char* key) {
        return GetValue().ToObject().GetMap().erase(key) != 0;
    }

    Iterator erase(const Iterator& iter) {
        auto new_iter = GetValue().ToObject().GetMap().erase(iter.obj_iter_);
        return Iterator{ this , new_iter };
    }

    bool IsValid() const noexcept {
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

    bool IsNull() const noexcept {
        if (!IsValid()) return false;
        return GetValue().IsNull();
    }

    bool IsBoolean() const noexcept {
        if (!IsValid()) return false;
        return GetValue().IsBoolean();
    }

    bool Boolean() const noexcept {
        return GetValue().ToBoolean().Get();
    }

    bool IsNumber() const noexcept {
        if (!IsValid()) return false;
        return GetValue().IsNumberInt() || GetValue().IsNumberFloat();
    }

    int64_t Int() const noexcept {
        return GetValue().ToNumberInt().Get();
    }
#ifndef YUJSON_DISABLE_FLOAT
    double Float() const noexcept {
        return GetValue().ToNumberFloat().Get();
    }
#endif

    bool IsString() const noexcept {
        if (!IsValid()) return false;
        return GetValue().IsString();
    }

    std::string String() const noexcept {
        return GetValue().ToString().Get();
    }

    bool IsArray() const noexcept {
        if (!IsValid()) return false;
        return GetValue().IsArray();
    }

    bool IsObject() const noexcept {
        if (!IsValid()) return false;
        return GetValue().IsObject();
    }


    int64_t ConvertToInt(int64_t defalut_int = 0) const {
        if (!IsValid()) return defalut_int;
        switch (GetValue().Type()) {
        case value::ValueType::kNumberInt: {
            return GetValue().GetNumberInt().Get();
        }
#ifndef YUJSON_DISABLE_FLOAT
        case value::ValueType::kNumberFloat:
            return static_cast<int64_t>(GetValue().ToNumberFloat().Get());
#endif                   
        case value::ValueType::kString: {
            auto& str = GetValue().GetString().Get();
            if (str.empty() || str[0] < '0' && str[0] > '9') return 0;
            return std::stoll(str);
        }
        case value::ValueType::kNull: {
            return defalut_int;
        }
        case value::ValueType::kBoolean: {
            return GetValue().GetBoolean().Get() ? 1 : 0;
        }
        default: {
            throw value::ValueTypeError("Object and Array cannot be converted to Int");
        }
        }
    }

#ifndef YUJSON_DISABLE_FLOAT
    double ConvertToFloat(double default_float = 0.0) {
        if (!IsValid()) return default_float;
        switch (GetValue().Type()) {
        case value::ValueType::kNumberInt: {
            return static_cast<double>(GetValue().GetNumberInt().Get());
        }
        case value::ValueType::kNumberFloat:
            return GetValue().ToNumberFloat().Get();             
        case value::ValueType::kString: {
            auto& str = GetValue().GetString().Get();
            if (str.empty() || str[0] < '0' && str[0] > '9') return 0;
            return std::stod(str);
        }
        case value::ValueType::kNull: {
            return default_float;
        }
        default: {
            throw value::ValueTypeError("Object and Array cannot be converted to Float");
        }
        }
    }
#endif

    std::string ConvertToString(std::string default_str = "") const {
        if (!IsValid()) return default_str;
        switch (GetValue().Type()) {
        case value::ValueType::kNumberInt: {
            return std::to_string(GetValue().GetNumberInt().Get());
        }
#ifndef YUJSON_DISABLE_FLOAT
        case value::ValueType::kNumberFloat:
            return std::to_string(GetValue().ToNumberFloat().Get());
#endif    
        case value::ValueType::kString: {
            return GetValue().GetString().Get().c_str();
        }
        case value::ValueType::kNull: {
            return default_str;
        }
        case value::ValueType::kBoolean: {
            return GetValue().GetBoolean().Get() ? "true" : "false";
        }
        default: {
            throw value::ValueTypeError("Object and Array cannot be converted to String");
        }
        }
    }

private:
    template <typename T>
    T& Get() noexcept {
        return *static_cast<T*>(this->get());
    }

    value::ValueInterface& GetValue() const {
        return **this;
    }

private:
    std::string StrEscape(const std::string& str) const {
        std::string new_str;
        bool escape = false;
        for (auto c : str) {
            if (escape) {
                escape = false;
                new_str += '\\';
                if (c != '\\') {
                    new_str += c;
                }
            }
            else if (c == '\\') {
                escape = true;
            }
            else if (c == '"') {
                new_str += R"(\")";
            }
            else if (c == '/') {
                new_str += R"(\/")";
            }
            else if (c == '\b') {
                new_str += R"(\b)";
            }
            else if (c == '\r') {
                new_str += R"(\r)";
            }
            else if (c == '\n') {
                new_str += R"(\n)";
            }
            else if (c == '\t') {
                new_str += R"(\t)";
            }
            else {
                new_str += c;
            }
        }
        return new_str;
    }

    void Print(value::ValueInterface* value, bool format, size_t level, std::string* jsonStr) const {
        std::string indent;
        if (format) {
            indent = std::string(level * kIndent, ' ');
        }

        switch (value->Type()) {
        case value::ValueType::kNull: {
            *jsonStr += "null";
            break;
        }
        case value::ValueType::kBoolean: {
            *jsonStr += value->GetBoolean().Get() ? "true" : "false";
            break;
        }
        case value::ValueType::kNumberInt: {
            auto str = std::to_string(value->GetNumberInt().Get());
            *jsonStr += str;
            break;
        }value->GetNumberInt();
#ifndef YUJSON_DISABLE_FLOAT
        case value::ValueType::kNumberFloat: {
            auto str = std::to_string(value->GetNumberFloat().Get());
            *jsonStr += str;
            break;
        }
#endif 
        case value::ValueType::kString: {
            auto str = StrEscape(value->GetString().Get());
            *jsonStr += "\"" + str + "\"";
            break;
        }
        case value::ValueType::kArray: {
            *jsonStr += '[';
            if (format) {
                indent += std::string(kIndent, ' ');
            }
            const auto& arr = value->GetArray().GetVector();
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
                indent += std::string(kIndent, ' ');
            }

            const auto& obj = value->GetObject().GetMap();
            int i = 0;
            for (const auto& it : obj) {
                if (format) {
                    *jsonStr += '\n' + indent;
                }
                auto key = StrEscape(it.first);
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
    static constexpr size_t kIndent = 4;
};

} // namespace yuJson

#endif // YUJSON_JSON_HPP_