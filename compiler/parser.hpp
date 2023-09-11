#ifndef YUJSON_COMPILER_PARSER_HPP_
#define YUJSON_COMPILER_PARSER_HPP_

#include <memory>

#include <yuJson/value/value.hpp>
#include <yuJson/compiler/lexer.hpp>

namespace yuJson {
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
    value::ValuePtr ParseValue() {
        Token token;
        if (!m_lexer->NextToken(&token)) {
            return nullptr;
        }
        switch (token.type) {
        case TokenType::kNull: {
            return YUJSON_STD make_unique<value::NullValue>();
        }
        case TokenType::kTrue: {
            return YUJSON_STD make_unique<value::BooleanValue>(true);
        }
        case TokenType::kFalse: {
            return YUJSON_STD make_unique<value::BooleanValue>(false);
        }
        case TokenType::kNumberInt: {
            return YUJSON_STD make_unique<value::NumberIntValue>(YUJSON_STD stoll(token.str));
        }
#ifndef YUJSON_DISABLE_FLOAT
        case TokenType::kNumberFloat: {
            return YUJSON_STD make_unique<value::NumberFloatValue>(YUJSON_STD stod(token.str));
        }
#endif
        case TokenType::kString: {
            return YUJSON_STD make_unique<value::StringValue>(token.str);
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

private:
    value::ArrayPtr ParseArray() {
        value::ArrayPtr array = YUJSON_STD make_unique<value::ArrayValue>();
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

    value::ObjectPtr ParseObject() {
        value::ObjectPtr object = YUJSON_STD make_unique<value::ObjectValue>();
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
        YUJSON_STD string key = token.str;
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
            YUJSON_STD string key = token.str;

            m_lexer->NextToken(&token);
            if (token.type != TokenType::kColon) {
                return nullptr;
            }
            object->Set(key, ParseValue());
        } while (true);
        if (!m_lexer->MatchToken(TokenType::kRcurly)) {
            return nullptr;
        }
        return object;
    }

private:
    Lexer* m_lexer;

};

} // namespace compiler
} // namespace yuJson

#endif // YUJSON_COMPILER_PARSER_HPP_