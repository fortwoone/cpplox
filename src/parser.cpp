//
// Created by fortwoone on 31/03/2025.
//

#include "parser.hpp"


namespace lox::parser{
    void parse(const string& file_contents){
        bool contains_errors = false;

        vector<Token> tokens = tokenize(file_contents, &contains_errors);

        size_t token_count = tokens.size();
        size_t idx = 0;
        for (const auto& token: tokens){
            switch (token.get_token_type()){
                case TokenType::TRUE:
                    cout << "true";
                    break;
                case TokenType::FALSE:
                    cout << "false";
                    break;
                case TokenType::NIL:
                    cout << "nil";
                    break;
                case TokenType::LEFT_PAREN:
                    cout << "(group ";
                    break;
                case TokenType::RIGHT_PAREN:
                    cout << ")";
                    break;
                case TokenType::NUMBER:
                case TokenType::STRING:
                    cout << token.get_literal_formatted_value();
                    break;
                default:
                    break;
            }
        }
    }
}
