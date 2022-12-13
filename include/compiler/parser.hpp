#ifndef PARSER_H_
#define PARSER_H_

#include <exception>
#include <memory>

#include "compiler\lexer.hpp"
#include "ast\value.hpp"

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

class ParserError : public std::exception {
public:
    ParserError(const char* msg) : std::exception(msg) { }
};


class Parser {
public:
    Parser(Lexer* lexer) : m_lexer(lexer) { }

public:
    std::unique_ptr<Json> ParseJson() {
        return std::make_unique<Json>(ParseValue());
    }

private:
    std::unique_ptr<ast::Value> ParseValue() {
        Token token = m_lexer->NextToken();
        switch (token.type) {
        case TokenType::kNull: {
            return std::make_unique<ast::Null>();
        }
        case TokenType::kTrue: {
            return std::make_unique<ast::Boolean>(true);
        }
        case TokenType::kFalse: {
            return std::make_unique<ast::Boolean>(false);
        }
        case TokenType::kNumber: {
            return std::make_unique<ast::Number>(atoi(token.str.c_str()));
        }
        case TokenType::kString: {
            return std::make_unique<ast::String>(token.str);
        }
        }

        if (token.type == TokenType::kLbrack) {
            return ParseArray();
        }

        if (token.type == TokenType::kLcurly) {
            return ParseObject();
        }


        throw ParserError("invalid token");

    }

    std::unique_ptr<ast::Array> ParseArray() {

        std::unique_ptr<ast::Array> array = std::make_unique<ast::Array>();

        Token token = m_lexer->LookAhead();
        if (token.type == TokenType::kRbrack) {
            m_lexer->NextToken();
            return array;
        }

        array->PushBack(ParseValue());

        do {
            Token token = m_lexer->LookAhead();
            if (token.type != TokenType::kComma) {
                break;
            }
            m_lexer->NextToken();
            array->PushBack(ParseValue());

        } while (true);

        m_lexer->MatchToken(TokenType::kRbrack);

        return array;
    }

    std::unique_ptr<ast::Object> ParseObject() {
        std::unique_ptr<ast::Object> object = std::make_unique<ast::Object>();

        Token token = m_lexer->NextToken();
        if (token.type == TokenType::kRcurly) {
            return object;
        }

        if (token.type != TokenType::kString) {
            throw ParserError("object key not string");
        }

        std::string key = token.str;

        token = m_lexer->NextToken();
        if (token.type != TokenType::kColon) {
            throw ParserError("object pair not colon separation");
        }

        object->Insert(key, ParseValue());

        do {
            Token token = m_lexer->LookAhead();
            if (token.type != TokenType::kComma) {
                break;
            }
            m_lexer->NextToken();

            token = m_lexer->NextToken();
            std::string key = token.str;

            token = m_lexer->NextToken();
            if (token.type != TokenType::kColon) {
                throw ParserError("object pair not colon separation");
            }

            object->Insert(key, ParseValue());

        } while (true);

        m_lexer->MatchToken(TokenType::kRcurly);

        return object;
    }

private:
    Lexer* m_lexer;

};

} // namespace compiler
} // namespace yuJson

#endif // PARSER_H_