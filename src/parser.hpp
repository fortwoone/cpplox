//
// Created by fortwoone on 31/03/2025.
//

#pragma once
#include "tokenizer.hpp"
#include "ast.hpp"
#include "exceptions.hpp"
#include "callable.hpp"
#include "env.hpp"
#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <utility>


namespace lox::parser{
    using lox::ubyte;

    using lox::ast::get_litexpr_tp_from_token_type;
    using lox::ast::get_op_from_token;
    using lox::callable::EvalResult;
    using lox::callable::CallablePtr;
    using lox::callable::is_number;
    using lox::callable::is_string;
    using lox::callable::is_boolean;
    using lox::callable::is_callable;
    using lox::env::Environment;

    using lox::tokenizer::literals::NumberLiteral;
    using lox::tokenizer::token::Token;
    using lox::tokenizer::token::TokenType;
    using lox::tokenizer::tokenize;

    using std::any_of;
    using std::cout;
    using std::cerr;
    using std::dynamic_pointer_cast;
    using std::endl;
    using std::exception;
    using std::fixed;
    using std::get;
    using std::holds_alternative;
    using std::initializer_list;
    using std::invalid_argument;
    using std::make_shared;
    using std::move;
    using std::nullptr_t;
    using std::runtime_error;
    using std::setprecision;
    using std::shared_ptr;
    using std::unitbuf;


    using ExprPtr = shared_ptr<ast::Expr>;
    using StmtPtr = shared_ptr<ast::Statement>;

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

        // region Expression methods
        [[nodiscard]] ExprPtr get_expr();

        [[nodiscard]] ExprPtr get_assignment();

        [[nodiscard]] ExprPtr get_or();

        [[nodiscard]] ExprPtr get_and();

        [[nodiscard]] ExprPtr get_equality();

        [[nodiscard]] ExprPtr get_comparison();

        [[nodiscard]] ExprPtr get_term();

        [[nodiscard]] ExprPtr get_factor();

        [[nodiscard]] ExprPtr get_unary();

        [[nodiscard]] ExprPtr get_call();

        [[nodiscard]] ExprPtr get_call_end(const ExprPtr& callee);

        [[nodiscard]] ExprPtr get_primary();
        // endregion

        // region Statement methods
        [[nodiscard]] StmtPtr get_print_statement();
        [[nodiscard]] StmtPtr get_expr_statement();
        [[nodiscard]] vector<StmtPtr> get_block_stmt();
        [[nodiscard]] StmtPtr get_while_stmt();
        [[nodiscard]] StmtPtr get_return_stmt();
        [[nodiscard]] StmtPtr get_if_statement();
        [[nodiscard]] StmtPtr get_for_statement();
        [[nodiscard]] StmtPtr get_statement();
        [[nodiscard]] StmtPtr get_var_declaration();
        [[nodiscard]] StmtPtr get_func_decl(bool is_method);
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

            ExprPtr parse_old();
            vector<StmtPtr> parse();

            ubyte evaluate();
    };

    ExprPtr parse(const string& file_contents);

    ubyte evaluate(const string& file_contents);
}
