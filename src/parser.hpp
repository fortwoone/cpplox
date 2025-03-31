//
// Created by fortwoone on 31/03/2025.
//

#pragma once
#include "tokenizer.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace lox{
    namespace parser{
        using lox::tokenizer::token::Token;
        using lox::tokenizer::token::TokenType;
        using lox::tokenizer::tokenize;
        using std::cout;
        using std::endl;
        using std::string;
        using std::unitbuf;
        using std::vector;

        namespace expr{

        }

        void parse(const string& file_contents);
    }
}