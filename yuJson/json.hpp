#ifndef YUJSON_JSON_H_
#define YUJSON_JSON_H_

#include <string>
#include <memory>

#include <yuJson/compiler/parser.hpp>
#include <yuJson/value/value.hpp>

namespace yuJson {

class Json {
public:
	Json() noexcept { }
	explicit Json(std::unique_ptr<value::Value> value) : m_value(nullptr) {
		m_value = std::move(value);
	}
	~Json() noexcept { }

	Json(const Json&) = delete;
	void operator=(const Json&) = delete;

	bool IsValid() {
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
	T& Get() {
		return *(T*)m_value.get();
	}

	value::Null& GetNull() noexcept {
		return *(value::Null*)m_value.get();
	}

	value::Boolean& GetBoolean() noexcept {
		return *(value::Boolean*)m_value.get();
	}

	value::Number& GetNumber() noexcept {
		return *(value::Number*)m_value.get();
	}

	value::String& GetString() noexcept {
		return *(value::String*)m_value.get();
	}

	value::Array& GetArray() noexcept {
		return *(value::Array*)m_value.get();
	}

	value::Object& GetObject() noexcept {
		return *(value::Object*)m_value.get();
	}

	void Set(std::unique_ptr<value::Value> value) {
		m_value = std::move(value);
	}

	//template <typename T>
	//void Set(T&& val) {
	//	m_value = std::make_unique<T>(std::move(val));
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
			*jsonStr += "\"" + static_cast<value::String*>(value)->Get() + "\"";
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
				*jsonStr += '\"' + it.first + "\":";

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