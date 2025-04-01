//
// Created by fortwoone on 31/03/2025.
//

#pragma once
#include "tokenizer.hpp"
#include <algorithm>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>


namespace lox::parser{
    using ubyte = uint8_t;

    using lox::tokenizer::token::Token;
    using lox::tokenizer::token::TokenType;
    using lox::tokenizer::tokenize;

    using std::any_of;
    using std::cout;
    using std::cerr;
    using std::endl;
    using std::exception;
    using std::get;
    using std::holds_alternative;
    using std::initializer_list;
    using std::invalid_argument;
    using std::make_unique;
    using std::move;
    using std::ostringstream;
    using std::string;
    using std::unique_ptr;
    using std::unitbuf;
    using std::unordered_map;
    using std::variant;
    using std::vector;

    class parse_error: public exception{
        ubyte return_code;
        const char* message;

        public:
            parse_error(ubyte return_code, const char* message): return_code(return_code), message(message){}
            [[nodiscard]] const char* what() const noexcept override{
                return message;
            }
            [[nodiscard]] ubyte get_return_code() const noexcept{
                return return_code;
            }
    };

    namespace ast{
        using EvalResult = variant<double, bool, string>;

        class Expr{
            public:
                virtual ~Expr() = default;
                [[nodiscard]] virtual string to_string() const = 0;
                [[nodiscard]] virtual EvalResult evaluate() const = 0;
        };


        enum class LiteralExprType: ubyte{
            TRUE,
            FALSE,
            NIL,
            STRING,
            NUMBER
        };


        // Literals
        class LiteralExpr: public Expr{
            string value;
            public:
                LiteralExprType expr_type;

                explicit LiteralExpr(LiteralExprType type): expr_type(type), value(""){}
                explicit LiteralExpr(LiteralExprType type, const string& value): expr_type(type), value(value){}

                [[nodiscard]] string to_string() const final;
                [[nodiscard]] EvalResult evaluate() const final;
        };

        enum class Operator: ubyte{
            PLUS,
            MINUS,
            STAR,
            SLASH,
            BANG,
            EQUALITY,
            INEQUALITY,
            LESS,
            GREATER,
            LESS_EQUAL,
            GREATER_EQUAL
        };

        class BinaryExpr: public Expr{
            public:
                unique_ptr<Expr> left, right;
                Operator op;
                BinaryExpr(unique_ptr<Expr> left, Operator op, unique_ptr<Expr> right)
                    : left(std::move(left)), op(op), right(std::move(right)){}

                [[nodiscard]] string to_string() const final;

                [[nodiscard]] EvalResult evaluate() const final;
        };

        class GroupExpr: public Expr{
            public:
                unique_ptr<Expr> expr;

                explicit GroupExpr(unique_ptr<Expr> expression): expr(std::move(expression)){}

                [[nodiscard]] string to_string() const final{
                    return "(group " + expr->to_string() + ")";
                }

                [[nodiscard]] EvalResult evaluate() const final{
                    return expr->evaluate();
                }
        };

        class UnaryExpr: public Expr{
            public:
                Operator op;
                unique_ptr<Expr> operand;

                UnaryExpr(Operator operat, unique_ptr<Expr> expression): op(operat), operand(std::move(expression)){}

                [[nodiscard]] string to_string() const final;

                [[nodiscard]] EvalResult evaluate() const final;
        };
    }

    class Parser{
        vector<Token> tokens;
        size_t current_idx = 0;

        [[nodiscard]] Token& peek(){
            return tokens.at(current_idx);
        }

        [[nodiscard]] Token& previous(){
            return tokens.at(current_idx - 1);
        }

        [[nodiscard]] bool is_at_end(){
            return peek().get_token_type() == TokenType::EOF_TOKEN;
        }

        Token& advance();

        [[nodiscard]] bool check(TokenType type);

        [[nodiscard]] bool match(initializer_list<TokenType> token_types);

        [[nodiscard]] bool match(TokenType tp){
            return match({tp});
        }

        [[nodiscard]] unique_ptr<ast::Expr> get_expr();

        [[nodiscard]] unique_ptr<ast::Expr> get_equality();

        [[nodiscard]] unique_ptr<ast::Expr> get_comparison();

        [[nodiscard]] unique_ptr<ast::Expr> get_term();

        [[nodiscard]] unique_ptr<ast::Expr> get_factor();

        [[nodiscard]] unique_ptr<ast::Expr> get_unary();

        [[nodiscard]] unique_ptr<ast::Expr> get_primary();

        Token& consume(TokenType token_type, const string& message){
            if (check(token_type)){
                return advance();
            }
            throw invalid_argument(message);
        }

        public:
            explicit Parser(vector<Token>& token_vec);

            unique_ptr<ast::Expr> parse();

            ubyte evaluate();
    };

    unique_ptr<ast::Expr> parse(const string& file_contents);

    ubyte evaluate(const string& file_contents);
}
