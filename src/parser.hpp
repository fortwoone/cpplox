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

                [[nodiscard]] EvalResult evaluate() const final{
                    switch (expr_type){
                        case LiteralExprType::TRUE:
                            return true;
                        case LiteralExprType::FALSE:
                            return false;
                        case LiteralExprType::NIL:
                            return "nil";
                        case LiteralExprType::NUMBER:
                            return stod(value);
                        default:
                            return value;
                    }
                }
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

#               define as_double get<double>
#               define as_bool get<bool>

                [[nodiscard]] EvalResult evaluate() const final{
                    using enum Operator;
                    EvalResult left_result = left->evaluate(), right_result = right->evaluate();
                    bool two_numbers = holds_alternative<double>(left_result) && holds_alternative<double>(right_result);
                    bool two_bools = holds_alternative<bool>(left_result) && holds_alternative<bool>(right_result);

                    switch (op){
                        case PLUS:
                            if (two_numbers){
                                return as_double(left_result) + as_double(right_result);
                            }
                            break;
                        case MINUS:
                            if (two_numbers){
                                return as_double(left_result) - as_double(right_result);
                            }
                            break;
                        case STAR:
                            if (two_numbers){
                                return as_double(left_result) * as_double(right_result);
                            }
                            break;
                        case SLASH:
                            if (two_numbers){
                                return as_double(left_result) / as_double(right_result);
                            }
                            break;
                        case LESS:
                            if (two_numbers){
                                return as_double(left_result) < as_double(right_result);
                            }
                            break;
                        case GREATER:
                            if (two_numbers){
                                return as_double(left_result) > as_double(right_result);
                            }
                            break;
                        case LESS_EQUAL:
                            if (two_numbers){
                                return as_double(left_result) <= as_double(right_result);
                            }
                            break;
                        case GREATER_EQUAL:
                            if (two_numbers){
                                return as_double(left_result) >= as_double(right_result);
                            }
                            break;
                        default:
                            break;
                    }
                    throw parse_error(70, "Unsupported operation.\0");
                }
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

                [[nodiscard]] string to_string() const final{
                    return "(" + _OP_TO_SYM.at(op) + " " + operand->to_string() + ")";
                }

                [[nodiscard]] EvalResult evaluate() const final{
                    EvalResult evaluated_operand = operand->evaluate();

                    if (holds_alternative<bool>(evaluated_operand)){
                        if (op == Operator::BANG){
                            return !as_bool(evaluated_operand);
                        }
                    }
                    else if (holds_alternative<double>(evaluated_operand)){
                        switch (op){
                            case Operator::MINUS:
                                return -as_double(evaluated_operand);
                            case Operator::BANG:
                                return false;
                            default:
                                break;
                        }
                    }
                    else{
                        switch (op){
                            case Operator::BANG:
                                return get<string>(evaluated_operand) == "nil";
                            default:
                                break;
                        }
                    }
                    throw parse_error(70, "Invalid operand for unary expression.\0");
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

            unique_ptr<ast::Expr> parse();

    };

    unique_ptr<ast::Expr> parse(const string& file_contents, bool* contains_errors);
}
