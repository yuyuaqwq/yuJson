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
            bool escape = false;
            while (c = NextChar()) {
                if (c == '\\') {
                    escape = true;
                    continue;
                }
                if (escape == false && c == '\"') {
                    break;
                }
                if (escape) {
                    escape = false;
                    if (c == '\"' || c == '/' || c == '\\') {
                        token->str += c;
                    }
                    else if (c == 'b') {
                        token->str += '\b';
                    }
                    else if (c == 'f') {
                        token->str += '\f';
                    }
                    else if (c == 'n') {
                        token->str += '\n';
                    }
                    else if (c == 'r') {
                        token->str += '\r';
                    }
                    else if (c == 'u') {
                        auto make_unicode_hex = [&](int& hex) -> bool {
                            YUJSON_STD string hex_str;
                            for (size_t i = 0; i < 4; i++) {
                                auto hex_c = NextChar();
                                if (hex_c) {
                                    hex_str += hex_c;
                                }
                                else {
                                    return false;
                                }
                            }
                            hex = YUJSON_STD stoi(hex_str, nullptr, 16);
                            return true;
                        };
                        
                        int hex_high;
                        if (!make_unicode_hex(hex_high)) {
                            return false;
                        }
                        int codepoint;
                        if (hex_high >= 0xd800 && hex_high <= 0xdbff) {
                            if (!MatchStr("\\u")) {
                                return false;
                            }
                            int hex_low;
                            if (!make_unicode_hex(hex_low)) {
                                return false;
                            }
                            if (hex_low < 0xdc00 || hex_low > 0xe000) {
                                return false;
                            }
                            codepoint = 0x10000 + (hex_high - 0xD800) * 0x400 + (hex_low - 0xDC00);
                        }
                        else {
                            codepoint = hex_high;
                        }

                        if (codepoint >= 0x10000 && codepoint <= 0x1ffff) {
                            token->str += 0xf0 | ((codepoint >> 18) & 0x07);
                            token->str += 0x80 | ((codepoint >> 12) & 0x3f);
                            token->str += 0x80 | ((codepoint >> 6) & 0x3f);
                            token->str += 0x80 | ((codepoint) & 0x3f);
                        }
                        else if (codepoint >= 0x800 && codepoint <= 0xffff) {
                            token->str += 0xe0 | ((codepoint >> 12) & 0x0f);
                            token->str += 0x80 | ((codepoint >> 6) & 0x3f);
                            token->str += 0x80 | ((codepoint) & 0x3f);
                        }
                        else if (codepoint < 0x7ff) {
                            token->str += 0xe0 | ((codepoint >> 6) & 0x0f);
                            token->str += 0x80 | (codepoint & 0x3f);
                        }
                        else if (codepoint < 0x7f) {
                            token->str += codepoint;
                        }
                        else {
                            return false;
                        }
                        
                    }
                }
                else {
                    token->str += c;
                }
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