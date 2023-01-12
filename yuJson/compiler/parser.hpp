#ifndef YUJSON_PARSER_H_
#define YUJSON_PARSER_H_

#include <memory>

#include <yuJson/compiler/lexer.hpp>
#include <yuJson/value/value.hpp>

namespace yuJson {

class Json;

namespace compiler {

/* EBNF
json -> value
value -> boolean | number | string | object | array | "null"
boolean -> "true" | "false"
number -> integer { "." integer }
string -> '"' ... '"'
object -> "{" [string ":" value {"," string ":" value} ] "}"
array -> "[" [value {"," value} ] "]"
*/

class Parser {
public:
    Parser(Lexer* lexer) : m_lexer(lexer) { }

public:
    std::unique_ptr<Json> ParseJson() {
        return std::make_unique<Json>(ParseValue());
    }

private:
    std::unique_ptr<value::Value> ParseValue() {
		Token token;
		if (!m_lexer->NextToken(&token)) {
			return nullptr;
		}
        switch (token.type) {
        case TokenType::kNull: {
            return std::make_unique<value::Null>();
        }
        case TokenType::kTrue: {
            return std::make_unique<value::Boolean>(true);
        }
        case TokenType::kFalse: {
            return std::make_unique<value::Boolean>(false);
        }
        case TokenType::kNumber: {
            return std::make_unique<value::Number>(atoi(token.str.c_str()));
        }
        case TokenType::kString: {
            return std::make_unique<value::String>(token.str);
        }
        }

        if (token.type == TokenType::kLbrack) {
            return ParseArray();
        }

        if (token.type == TokenType::kLcurly) {
            return ParseObject();
        }

		return nullptr;

    }

    std::unique_ptr<value::Array> ParseArray() {
        std::unique_ptr<value::Array> array = std::make_unique<value::Array>();

		Token token;
		if (!m_lexer->LookAhead(&token)) {
			return nullptr;
		}
        if (token.type == TokenType::kRbrack) {
            m_lexer->NextToken(nullptr);
            return array;
        }

        array->Pushback(ParseValue());

        do {
			Token token;
			if (!m_lexer->LookAhead(&token)) {
				return nullptr;
			}
            if (token.type != TokenType::kComma) {
                break;
            }
            m_lexer->NextToken(nullptr);
            array->Pushback(ParseValue());

        } while (true);

		if (!m_lexer->MatchToken(TokenType::kRbrack)) {
			return nullptr;
		}

        return array;
    }

    std::unique_ptr<value::Object> ParseObject() {
        std::unique_ptr<value::Object> object = std::make_unique<value::Object>();

		Token token;
		if (!m_lexer->NextToken(&token)) {
			return nullptr;
		}
        if (token.type == TokenType::kRcurly) {
            return object;
        }

        if (token.type != TokenType::kString) {
			return nullptr;
        }

        std::string key = token.str;

        

        if (!m_lexer->NextToken(&token) || token.type != TokenType::kColon) {
			return nullptr;
        }

        object->Set(key, ParseValue());

        do {
			Token token;
			
			if (!m_lexer->LookAhead(&token)) {
				return nullptr;
			}
            if (token.type != TokenType::kComma) {
                break;
            }
            m_lexer->NextToken(nullptr);

            m_lexer->NextToken(&token);
            std::string key = token.str;

            m_lexer->NextToken(&token);
            if (token.type != TokenType::kColon) {
				return nullptr;
            }

            object->Set(key, ParseValue());

        } while (true);

        m_lexer->MatchToken(TokenType::kRcurly);

        return object;
    }

private:
    Lexer* m_lexer;

};

} // namespace compiler
} // namespace yuJson

#endif // YUJSON_PARSER_H_