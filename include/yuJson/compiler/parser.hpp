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
    Parser(Lexer* lexer) : lexer_(lexer) { }

public:
    value::ValuePtr ParseValue() {
        Token token;
        if (!lexer_->NextToken(&token)) {
            return nullptr;
        }
        switch (token.type) {
        case TokenType::kNull: {
            return std::make_unique<value::NullValue>();
        }
        case TokenType::kTrue: {
            return std::make_unique<value::BooleanValue>(true);
        }
        case TokenType::kFalse: {
            return std::make_unique<value::BooleanValue>(false);
        }
        case TokenType::kNumberInt: {
            return std::make_unique<value::NumberIntValue>(std::stoll(token.str));
        }
#ifndef YUJSON_DISABLE_FLOAT
        case TokenType::kNumberFloat: {
            return std::make_unique<value::NumberFloatValue>(std::stod(token.str));
        }
#endif
        case TokenType::kString: {
            return std::make_unique<value::StringValue>(token.str);
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
        value::ArrayPtr array = std::make_unique<value::ArrayValue>();
        Token token;
        if (!lexer_->LookAhead(&token)) {
            return nullptr;
        }
        if (token.type == TokenType::kRbrack) {
            lexer_->NextToken(nullptr);
            return array;
        }
        array->PushBack(ParseValue());
        do {
            Token token;
            if (!lexer_->LookAhead(&token)) {
                return nullptr;
            }
            if (token.type != TokenType::kComma) {
                break;
            }
            lexer_->NextToken(nullptr);
            array->PushBack(ParseValue());

        } while (true);
        if (!lexer_->MatchToken(TokenType::kRbrack)) {
            return nullptr;
        }
        return array;
    }

    value::ObjectPtr ParseObject() {
        value::ObjectPtr object = std::make_unique<value::ObjectValue>();
        Token token;
        if (!lexer_->NextToken(&token)) {
            return nullptr;
        }
        if (token.type == TokenType::kRcurly) {
            return object;
        }
        if (token.type != TokenType::kString) {
            return nullptr;
        }
        std::string key = token.str;
        if (!lexer_->NextToken(&token) || token.type != TokenType::kColon) {
            return nullptr;
        }
        object->Set(key, ParseValue());
        do {
            Token token;
            if (!lexer_->LookAhead(&token)) {
                return nullptr;
            }
            if (token.type != TokenType::kComma) {
                break;
            }
            lexer_->NextToken(nullptr);

            lexer_->NextToken(&token);
            std::string key = token.str;

            lexer_->NextToken(&token);
            if (token.type != TokenType::kColon) {
                return nullptr;
            }
            object->Set(key, ParseValue());
        } while (true);
        if (!lexer_->MatchToken(TokenType::kRcurly)) {
            return nullptr;
        }
        return object;
    }

private:
    Lexer* lexer_;

};

} // namespace compiler
} // namespace yuJson

#endif // YUJSON_COMPILER_PARSER_HPP_