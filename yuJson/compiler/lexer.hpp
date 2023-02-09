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

        if (c >= '0' && c <= '9') {
            token->type = TokenType::kNumber;
            token->str = "";
            do {
                if (c >= '0' && c <= '9') {
                    token->str += std::string(1, c);
                }
                else {
                    SkipChar(-1);
                    break;
                }
            } while (c = NextChar());
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
                    // 只处理引号的转义
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
    std::string m_src;
    size_t m_idx;
    Token m_nextToken;
};

} // namespace compiler
} // namespace yuJson

#endif // YUJSON_COMPILER_LEXER_H_