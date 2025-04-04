//
// Created by fortwoone on 31/03/2025.
//

#pragma once
#include "tokenizer.hpp"
#include "env.hpp"
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

    using lox::env::Environment;
    using lox::tokenizer::token::Token;
    using lox::tokenizer::token::TokenType;
    using lox::tokenizer::tokenize;

    using std::any_of;
    using std::cout;
    using std::cerr;
    using std::dynamic_pointer_cast;
    using std::endl;
    using std::exception;
    using std::get;
    using std::holds_alternative;
    using std::initializer_list;
    using std::invalid_argument;
    using std::make_shared;
    using std::move;
    using std::ostringstream;
    using std::string;
    using std::shared_ptr;
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
        using EvalResult = lox::env::VarValue;

        // region Expressions
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

                explicit LiteralExpr(LiteralExprType type): expr_type(type){}
                explicit LiteralExpr(LiteralExprType type, string value): expr_type(type), value(std::move(value)){}

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
                shared_ptr<Expr> left, right;
                Operator op;
                BinaryExpr(shared_ptr<Expr> left, Operator op, shared_ptr<Expr> right)
                    : left(std::move(left)), op(op), right(std::move(right)){}

                [[nodiscard]] string to_string() const final;

                [[nodiscard]] EvalResult evaluate() const final;
        };

        class GroupExpr: public Expr{
            public:
                shared_ptr<Expr> expr;

                explicit GroupExpr(shared_ptr<Expr> expression): expr(std::move(expression)){}

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
                shared_ptr<Expr> operand;

                UnaryExpr(Operator operat, shared_ptr<Expr> expression): op(operat), operand(std::move(expression)){}

                [[nodiscard]] string to_string() const final;

                [[nodiscard]] EvalResult evaluate() const final;
        };

        class AbstractVarExpr: public Expr{
            protected:
                string name;
                shared_ptr<Environment> env;

            public:
                AbstractVarExpr(const string& name, const shared_ptr<Environment>& env);

                [[nodiscard]] string get_name() const{
                    return name;
                };

                [[nodiscard]] string to_string() const final{
                    return name;
                };

                [[nodiscard]] EvalResult evaluate() const override = 0;

                void set_env(const shared_ptr<Environment>& env);
        };

        class VariableExpr: public AbstractVarExpr{
            public:
                explicit VariableExpr(const Token& id_token, const shared_ptr<Environment>& new_env);

                [[nodiscard]] EvalResult evaluate() const final;
        };

        class AssignmentExpr: public AbstractVarExpr{
            shared_ptr<Expr> value;

            public:
                AssignmentExpr(string name, shared_ptr<Expr> value, const shared_ptr<Environment>& env);

                [[nodiscard]] shared_ptr<Expr> get_value() const{
                    return value;
                };

                [[nodiscard]] EvalResult evaluate() const final;
        };
        // endregion

        // Base class for statements. A statement is an instruction executed by the interpreter.
        class Statement{
            public:
                virtual ~Statement() = default;
                virtual void execute() const = 0;
        };

        class StatementWithExpr: public Statement{
            protected:
                shared_ptr<Expr> expr;
            public:
                explicit StatementWithExpr(shared_ptr<Expr> expr): expr(std::move(expr)){}

                [[nodiscard]] shared_ptr<Expr> get_expr() const{
                    return expr;
                }

                void execute() const override = 0;
        };

        class ExprStatement: public StatementWithExpr{
            public:
                explicit ExprStatement(shared_ptr<Expr> expr): StatementWithExpr(std::move(expr)){}

                void execute() const final;
        };

        class PrintStatement: public StatementWithExpr{
            public:
                explicit PrintStatement(shared_ptr<Expr> expr): StatementWithExpr(std::move(expr)){}

                void execute() const final;
        };

        class VariableStatement: public StatementWithExpr{
            string name;

            public:
                explicit VariableStatement(const string& name, shared_ptr<Expr> init_expr);

                [[nodiscard]] string get_name() const{
                    return name;
                }

                [[nodiscard]] shared_ptr<Expr> get_initialiser() const{
                    return expr;
                }

                void execute() const final;
        };

        class BlockStatement: public Statement{
            vector<shared_ptr<Statement>> statements;
            shared_ptr<Environment> env;

            void exec_var_stmt(const shared_ptr<VariableStatement>& var_stmt) const;

            public:
                explicit BlockStatement(vector<shared_ptr<Statement>> statements, const shared_ptr<Environment>& env);

                void set_env(const shared_ptr<Environment>& new_env);

                void execute() const final;
        };
    }

    using ExprPtr = shared_ptr<ast::Expr>;
    using StmtPtr = shared_ptr<ast::Statement>;

    class Parser{
        vector<Token> tokens;
        shared_ptr<Environment> env;
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

        // region Expression methods
        [[nodiscard]] ExprPtr get_expr();

        [[nodiscard]] ExprPtr get_assignment();

        [[nodiscard]] ExprPtr get_equality();

        [[nodiscard]] ExprPtr get_comparison();

        [[nodiscard]] ExprPtr get_term();

        [[nodiscard]] ExprPtr get_factor();

        [[nodiscard]] ExprPtr get_unary();

        [[nodiscard]] ExprPtr get_primary();
        // endregion

        // region Statement methods
        [[nodiscard]] StmtPtr get_print_statement();
        [[nodiscard]] StmtPtr get_expr_statement();
        [[nodiscard]] vector<StmtPtr> get_block_stmt();
        [[nodiscard]] StmtPtr get_statement();
        [[nodiscard]] StmtPtr get_var_declaration();
        [[nodiscard]] StmtPtr get_declaration();
        // endregion

        Token& consume(TokenType token_type, const string& message){
            if (check(token_type)){
                return advance();
            }
            throw invalid_argument(message);
        }

        public:
            explicit Parser(vector<Token> token_vec);
            Parser(vector<Token> token_vec, const shared_ptr<Environment>& env);

            ExprPtr parse_old();
            vector<StmtPtr> parse();

            ubyte evaluate();
    };

    ExprPtr parse(const string& file_contents);

    ubyte evaluate(const string& file_contents);
}
