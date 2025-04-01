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
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
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
    using std::initializer_list;
    using std::invalid_argument;
    using std::make_unique;
    using std::move;
    using std::string;
    using std::unique_ptr;
    using std::unitbuf;
    using std::unordered_map;
    using std::vector;

    namespace ast{
        class Expr{
            public:
                virtual ~Expr() = default;
                [[nodiscard]] virtual string to_string() const = 0;
        };

        enum class LiteralExprType: ubyte{
            TRUE,
            FALSE,
            NIL,
            STRING,
            NUMBER
        };

        const unordered_map<TokenType, LiteralExprType> _TOKEN_TO_LITEXPRTP{
            {TokenType::TRUE, LiteralExprType::TRUE},
            {TokenType::FALSE, LiteralExprType::FALSE},
            {TokenType::NIL, LiteralExprType::NIL},
            {TokenType::STRING, LiteralExprType::STRING},
            {TokenType::NUMBER, LiteralExprType::NUMBER},
        };

        // Literals
        class LiteralExpr: public Expr{
            string value;
            public:
                LiteralExprType expr_type;

                explicit LiteralExpr(LiteralExprType type): expr_type(type), value(""){}
                explicit LiteralExpr(LiteralExprType type, const string& value): expr_type(type), value(value){}
                [[nodiscard]] string to_string() const final{
                    switch (expr_type){
                        case LiteralExprType::TRUE:
                            return "true";
                        case LiteralExprType::FALSE:
                            return "false";
                        case LiteralExprType::NIL:
                            return "nil";
                        default:
                            return value;
                    }
                };
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

        const unordered_map<TokenType, Operator> _TOKEN_TO_OP{
            {TokenType::PLUS, Operator::PLUS},
            {TokenType::MINUS, Operator::MINUS},
            {TokenType::STAR, Operator::STAR},
            {TokenType::SLASH, Operator::SLASH},
            {TokenType::BANG, Operator::BANG},
            {TokenType::EQUAL_EQUAL, Operator::EQUALITY},
            {TokenType::BANG_EQUAL, Operator::INEQUALITY},
            {TokenType::LESS, Operator::LESS},
            {TokenType::GREATER, Operator::GREATER},
            {TokenType::LESS_EQUAL, Operator::LESS_EQUAL},
            {TokenType::GREATER_EQUAL, Operator::GREATER_EQUAL},
        };

        const unordered_map<Operator, string> _OP_TO_SYM{
            {Operator::PLUS, "+"},
            {Operator::MINUS, "-"},
            {Operator::STAR, "*"},
            {Operator::SLASH, "/"},
            {Operator::BANG, "!"},
            {Operator::EQUALITY, "=="},
            {Operator::INEQUALITY, "!="},
            {Operator::LESS, "<"},
            {Operator::GREATER, ">"},
            {Operator::LESS_EQUAL, "<="},
            {Operator::GREATER_EQUAL, ">="},
        };

        class BinaryExpr: public Expr{
            public:
                unique_ptr<Expr> left, right;
                Operator op;
                BinaryExpr(unique_ptr<Expr> left, Operator op, unique_ptr<Expr> right)
                    : left(std::move(left)), op(op), right(std::move(right)){}

                [[nodiscard]] string to_string() const final{
                    return "(" + _OP_TO_SYM.at(op) + " " + left->to_string() + " " + right->to_string() + ")";
                }
        };

        class GroupExpr: public Expr{
            public:
                unique_ptr<Expr> expr;

                explicit GroupExpr(unique_ptr<Expr> expression): expr(std::move(expression)){}

                [[nodiscard]] string to_string() const final{
                    return "(group " + expr->to_string() + ")";
                }
        };

        class UnaryExpr: public Expr{
            public:
                Operator op;
                unique_ptr<Expr> operand;

                UnaryExpr(Operator operat, unique_ptr<Expr> expression): op(operat), operand(std::move(expression)){}

                [[nodiscard]] string to_string() const final{
                    return "(" + _OP_TO_SYM.at(op) + " " + operand->to_string() + ")";
                }
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

            void parse();

    };

    bool parse(const string& file_contents);
}
