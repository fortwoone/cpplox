//
// Created by fortwoone on 08/04/2025.
//

#pragma once
#include "parser.hpp"
#include "resolver.hpp"
#include "interpreter.hpp"
#include "tokenizer.hpp"
#include <memory>
#include <string>

namespace lox::runner{
    using lox::interpreter::Interpreter;
    using lox::parser::Parser;
    using lox::resolver::Resolver;
    using lox::tokenizer::tokenize;

    using std::make_shared;
    using std::shared_ptr;
    using std::string;

    void run(const string& file_contents);
}
