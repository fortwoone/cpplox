//
// Created by fortwoone on 02/04/2025.
//

#pragma once
#include "tokenizer.hpp"
#include "parser.hpp"
#include "env.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace lox::interpreter{
    using lox::env::Environment;
    using lox::env::VarValue;
    using lox::parser::parse_error;
    using lox::parser::Parser;
    using lox::parser::ast::VariableStatement;
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
        shared_ptr<Environment> env;

        void execute_var_statement(const shared_ptr<VariableStatement>& var_stmt);

        public:
            explicit Interpreter(const string& file_contents);
            explicit Interpreter(const vector<StmtPtr>& statements);

            void run();
    };

    void run(const string& file_contents);
}
