//
// Created by fortwoone on 02/04/2025.
//

#pragma once
#include "tokenizer.hpp"
#include "parser.hpp"
#include <stdexcept>
#include <string>
#include <vector>

namespace lox::interpreter{
    using lox::parser::parse_error;
    using lox::parser::Parser;
    using lox::parser::StmtPtr;
    using lox::tokenizer::token::Token;
    using lox::tokenizer::tokenize;

    using std::exception;
    using std::runtime_error;
    using std::string;
    using std::vector;

    class Interpreter{
        vector<StmtPtr> statements;

        public:
            explicit Interpreter(const string& file_contents);
            explicit Interpreter(const vector<StmtPtr>& statements);

            void run();
    };

    void run(const string& file_contents);
}
