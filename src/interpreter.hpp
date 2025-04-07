//
// Created by fortwoone on 02/04/2025.
//

#pragma once
#include "tokenizer.hpp"
#include "parser.hpp"
#include "env.hpp"
#include "exceptions.hpp"
#include "callable.hpp"
#include "ast.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace lox::interpreter{
    namespace builtins = lox::callable::builtins;
    using lox::env::Environment;
    using lox::callable::VarValue;
    using lox::parser::Parser;
    using lox::ast::VariableStatement;
    using lox::parser::StmtPtr;
    using lox::tokenizer::token::Token;
    using lox::tokenizer::tokenize;

    using std::dynamic_pointer_cast;
    using std::exception;
    using std::make_shared;
    using std::runtime_error;
    using std::shared_ptr;
    using std::string;
    using std::vector;

    class Interpreter{
        vector<StmtPtr> statements;
        shared_ptr<Environment> globals;
        shared_ptr<Environment> env;

        void define_builtins();
        void execute_var_statement(const shared_ptr<VariableStatement>& var_stmt);

        public:
            explicit Interpreter(const string& file_contents);
            explicit Interpreter(const vector<StmtPtr>& statements);

            [[nodiscard]] shared_ptr<Environment> get_globals() const{
                return globals;
            }

            void run();
    };

    void run(const string& file_contents);
}
