//
// Created by fortwoone on 31/03/2025.
//

#include "parser.hpp"


namespace lox::parser{
    void parse(const string& file_contents){
        bool contains_errors = false;

        vector<Token> tokens = tokenize(file_contents, &contains_errors);

        for (const auto& token: tokens){
            switch (token.get_token_type()){
                case TokenType::TRUE:
                    cout << "true" << endl;
                    break;
                case TokenType::FALSE:
                    cout << "false" << endl;
                    break;
                case TokenType::NIL:
                    cout << "nil" << endl;
                    break;
                default:
                    break;
            }
        }
    }
}
