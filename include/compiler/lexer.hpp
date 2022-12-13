#ifndef LEXER_H_
#define LEXER_H_

#include <string>

namespace yuJson {
namespace compiler {

enum class TokenType {
    kNone = 0,
    kEof,
    kNull,
    kTrue,
    kFalse,
    kNumber,
    kString,
    kLbrack,
    kRbrack,
    kLcurly,
    kRcurly,
    kComma,
    kColon,
};

struct Token {
    TokenType type;
    std::string str;
};

class LexerError : public std::exception {
public:
    LexerError(const char* msg) : std::exception(msg) { }
};

class Lexer {
public:
    Lexer(const std::string& src) : m_src(src), m_idx(0) {
        m_nextToken.type = TokenType::kNone;
    }

public:
    char NextChar() noexcept {
        if (m_idx < m_src.size()) {
            return m_src[m_idx++];
        }
        return 0;
    }

    bool MatchStr(const char* str) noexcept {
        size_t len = strlen(str);
        if (m_idx + len <= m_src.size()) {
            int res = memcmp(str, m_src.c_str() + m_idx, len);
            if (res == 0) {
                m_idx += len;
                return true;
            }
        }
        return false;
    }

    void SkipChar(int c) noexcept {
        m_idx += c;
    }

    Token LookAhead() {
        if (m_nextToken.type == TokenType::kNone) {
            m_nextToken = NextToken();
        }
        return m_nextToken;

    }

    Token NextToken() {

        Token token;
        if (m_nextToken.type != TokenType::kNone) {
            token = m_nextToken;
            m_nextToken.type = TokenType::kNone;
            return token;
        }


        char c;

        while ((c = NextChar()) && c == ' ');

        if (c == 0) {
            token.type = TokenType::kEof;
            return token;
        }

        switch (c) {
        case '{':
            token.type = TokenType::kLcurly;
            return token;
        case '}':
            token.type = TokenType::kRcurly;
            return token;
        case '[':
            token.type = TokenType::kLbrack;
            return token;
        case ']':
            token.type = TokenType::kRbrack;
            return token;
        case ',':
            token.type = TokenType::kComma;
            return token;
        case ':':
            token.type = TokenType::kColon;
            return token;
        }

        if (c == 'n') {
            if (MatchStr("ull")) {
                token.type = TokenType::kNull;
                return token;
            }
        }
        if (c == 't') {
            if (MatchStr("rue")) {
                token.type = TokenType::kTrue;
                return token;
            }
        }
        if (c == 'f') {
            if (MatchStr("alse")) {
                token.type = TokenType::kFalse;
                return token;
            }
        }

        if (c >= '0' && c <= '9') {
            token.type = TokenType::kNumber;
            do {
                if (c >= '0' && c <= '9') {
                    token.str += std::string(1, c);
                }
                else {
                    SkipChar(-1);
                    break;
                }
            } while (c = NextChar());
            return token;
        }

        if (c == '\"') {
            token.type = TokenType::kString;
            while ((c = NextChar()) && c != '\"') {
                token.str += c;
            };
            if (c != '\"') {
                throw LexerError("cannot parse string");
            }
            return token;
        }

        throw LexerError("cannot parse token");

        return token;
    }

    void MatchToken(TokenType type) {
        Token token = LookAhead();
        if (token.type != type) {
            throw LexerError("cannot match token");
        }
        NextToken();
        return;
    }

private:
    std::string m_src;
    size_t m_idx;
    Token m_nextToken;
};

} // namespace compiler
} // namespace yuJson

#endif // LEXER_H_