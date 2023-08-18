#ifndef YUJSON_COMPILER_LEXER_H_
#define YUJSON_COMPILER_LEXER_H_

#include <string>

namespace yuJson {
namespace compiler {

enum class TokenType {
    kNone = 0,
    kEof,
    kNull,
    kTrue,
    kFalse,
    kNumberInt,
    kNumberFloat,
    kString,
    kLbrack,
    kRbrack,
    kLcurly,
    kRcurly,
    kComma,
    kColon,
    kMinus,
};

struct Token {
    TokenType type;
    YUJSON_STD string str;
};

class Lexer {
public:
    Lexer(const YUJSON_STD string& src) : m_src(src), m_idx(0) {
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

    bool LookAhead(Token* token) noexcept {
        if (m_nextToken.type == TokenType::kNone) {
            if (!NextToken(&m_nextToken)) {
                return false;
            }
        }
        *token = m_nextToken;
        return true;
    }

    bool NextToken(Token* token) noexcept {
        if (m_nextToken.type != TokenType::kNone) {
            if (token) {
                *token = m_nextToken;
            }
            m_nextToken.type = TokenType::kNone;
            return true;
        }

        char c;
        while ((c = NextChar()) && (c == ' ' || c == '\t' || c == '\r' || c == '\n'));

        if (c == 0) {
            token->type = TokenType::kEof;
            return true;
        }
        switch (c) {
        case '{':
            token->type = TokenType::kLcurly;
            return true;
        case '}':
            token->type = TokenType::kRcurly;
            return true;
        case '[':
            token->type = TokenType::kLbrack;
            return true;
        case ']':
            token->type = TokenType::kRbrack;
            return true;
        case ',':
            token->type = TokenType::kComma;
            return true;
        case ':':
            token->type = TokenType::kColon;
            return true;
        }

        if (c == 'n') {
            if (MatchStr("ull")) {
                token->type = TokenType::kNull;
                return true;
            }
        }
        if (c == 't') {
            if (MatchStr("rue")) {
                token->type = TokenType::kTrue;
                return true;
            }
        }
        if (c == 'f') {
            if (MatchStr("alse")) {
                token->type = TokenType::kFalse;
                return true;
            }
        }

        if (c >= '0' && c <= '9' || c == '-') {
            token->str = "";
            int i = 0;
            bool is_float = false;
            bool is_e = false;
            int e_pos = 0;
            do {
#ifdef YUJSON_ENABLE_FLOAT
                if (c >= '0' && c <= '9' || c == '-' && i == 0 || c == '+' && i == 0 || c == '.' && is_float == false) {
                    token->str += YUJSON_STD string(1, c);
                    if (c == '.') is_float = true;
                }
                else if (is_e == false && is_float && (c == 'e' || c == 'E')) {
                    token->str += YUJSON_STD string(1, c);
                    is_e = true;
                    e_pos = i;
                }
                else if (is_e == true && i == e_pos+1 && (c == '+' || c == '-')) {
                    token->str += YUJSON_STD string(1, c);
                }
#else
                if (c >= '0' && c <= '9' || c == '-' && i == 0 || c == '+' && i == 0) {
                    token->str += YUJSON_STD string(1, c);
                }
#endif
                else {
                     SkipChar(-1);
                     break;
                }
                ++i;
            } while (c = NextChar());
            auto end_char = token->str.at(token->str.size() - 1);
            if (is_float && (end_char < '0' || end_char > '9')) {
                return false;
            }
            token->type = is_float ? TokenType::kNumberFloat : TokenType::kNumberInt;
            return true;
        }

        if (c == '\"') {
            token->type = TokenType::kString;
            token->str = "";
            bool skip = false;
            while (c = NextChar()) {
                if (c == '\\') {
                    skip = true;
                    continue;
                }
                if (skip == false && c == '\"') {
                    break;
                }
                if (skip) { 
                    skip = false; 
                    if (c != '\"') {
                        token->str += "\\";
                    }
                }
                token->str += c;
            };
            if (c != '\"') {
                return false;
            }
            return true;
        }
        return false;
    }

    bool MatchToken(TokenType type) noexcept {
        Token token;
        if (!LookAhead(&token)) {
            return false;
        }
        if (token.type != type) {
            return false;
        }
        NextToken(nullptr);
        return true;
    }

private:
    YUJSON_STD string m_src;
    size_t m_idx;
    Token m_nextToken;
};

} // namespace compiler
} // namespace yuJson

#endif // YUJSON_COMPILER_LEXER_H_