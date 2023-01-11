#ifndef YUJSON_VALUE_JSON_H_
#define YUJSON_VALUE_JSON_H_

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

	void Parse(const char* jsonText) {
		compiler::Lexer lexer(jsonText);
		compiler::Parser parser(&lexer);
		auto tempJson = parser.ParseJson();
		m_value = std::move(tempJson->m_value);
	}

	std::string Print(bool format = true) const {
		std::string jsonStr;
		Print(m_value.get(), format, 0, &jsonStr);
		return jsonStr;
	}

	value::Value* Get() noexcept {
		return m_value.get();
	}

	void Set(std::unique_ptr<value::Value> value) {
		m_value = std::move(value);
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
			*jsonStr += std::to_string(static_cast<value::Number*>(value)->GetFloat());
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
			const auto& arr = static_cast<value::Array*>(value)->Get();
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

			const auto& obj = static_cast<value::Object*>(value)->Get();
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

#endif // YUJSON_VALUE_JSON_H_