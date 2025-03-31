//
// Created by fortwoone on 30/03/2025.
//

#pragma once
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace lox{
    namespace tokenizer{
        using ubyte = uint8_t;
        using ulong = uint64_t;
        using std::any_of;
        using std::cout;
        using std::cerr;
        using std::endl;
        using std::exit;
        using std::fixed;
        using std::ostringstream;
        using std::setprecision;
        using std::stoi;
        using std::stod;
        using std::string;
        using std::unordered_map;
        using std::unordered_set;
        using std::vector;

        namespace literals{
            enum class LiteralType: ubyte{
                NULL_LITERAL,
                STRING,
                NUMBER
            };

            class Literal{
                LiteralType lit_type;

                protected:
                    string orig_val;
                    Literal(LiteralType lit, const string& orig_val);

                public:
                    [[nodiscard]] LiteralType get_literal_type() const{
                        return lit_type;
                    }

                    [[nodiscard]] virtual string get_formatted_value() const;
            };

            class NullLiteral: public Literal{
                public:
                    NullLiteral();
                    [[nodiscard]] constexpr string get_formatted_value() const final;
            };

            class StringLiteral: public Literal{
                public:
                    explicit StringLiteral(const string& orig_val);

                    [[nodiscard]] string get_formatted_value() const final;
            };

            class NumberLiteral: public Literal{
                double val_as_dbl;
                size_t precision;

                public:
                    explicit NumberLiteral(const string& orig_val);
                    [[nodiscard]] string get_formatted_value() const final;
            };
        }

        namespace token{
            using literals::LiteralType;
            using literals::Literal;

            enum class TokenType: ubyte{
                LEFT_PAREN,
                RIGHT_PAREN,
                LEFT_BRACE,
                RIGHT_BRACE,
                DOT,
                STAR,
                MINUS,
                PLUS,
                SEMICOLON,
                COMMA,
                SLASH,
                EQUAL,
                BANG,
                LESS,
                GREATER,
                EQUAL_EQUAL,
                BANG_EQUAL,
                LESS_EQUAL,
                GREATER_EQUAL,
                STRING,
                NUMBER,
                IDENTIFIER,
                AND,
                CLASS,
                ELSE,
                FALSE,
                FOR,
                FUN,
                IF,
                NIL,
                OR,
                PRINT,
                RETURN,
                SUPER,
                THIS,
                TRUE,
                VAR,
                WHILE,
                EOF_TOKEN
            };

            class Token{
                TokenType token_type;
                string lexeme;
                Literal* literal; // Can be any of NullLiteral, StringLiteral or NumberLiteral depending on the token type.

                public:
                    Token(TokenType token_tp, const string& lexeme);
                    explicit Token(TokenType token_tp);
                    Token(TokenType token_tp, const string& lexeme, LiteralType treat_as);

                    ~Token(){
                        if (literal != nullptr) {
                            delete literal;
                            literal = nullptr;
                        }
                    }

                    [[nodiscard]] TokenType get_token_type() const{
                        return token_type;
                    }

                    [[nodiscard]] string get_lexeme() const{
                        return lexeme;
                    }

                    [[nodiscard]] string get_literal_formatted_value() const{
                        return literal->get_formatted_value();
                    }

                    void show_in_cli();
            };
        }

        namespace priv{
            bool is_token(const char& c);
            string get_token_name(const char& c);
            bool is_complex_token(const char& c);
            bool is_ignore_char(const char& c);
            bool is_digit(const char& c);
            bool is_identifier_char(const char& c);
            bool is_reserved_kw(const string& literal_str);
            string get_kw_name(const string& kw_name);
            void display_number(const string& number);
            void reset_precision();
        }

        vector<token::Token> tokenize(const string& file_contents, bool* contained_errors);
    }
}
