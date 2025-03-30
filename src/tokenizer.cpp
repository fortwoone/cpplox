//
// Created by fortwoone on 30/03/2025.
//

#include "tokenizer.hpp"

namespace tokenizer{
    const unordered_map<char, string> _TOKEN_NAMES{
            {'(', "LEFT_PAREN"},
            {')', "RIGHT_PAREN"},
            {'{', "LEFT_BRACE"},
            {'}', "RIGHT_BRACE"},
            {'.', "DOT"},
            {'*', "STAR"},
            {'-', "MINUS"},
            {'+', "PLUS"},
            {';', "SEMICOLON"},
            {',', "COMMA"},
            {'/', "SLASH"}
    };

    void tokenize(const string& file_contents){
        for (const auto& byte: file_contents){
#           if __cplusplus >= 202002L
            if (_TOKEN_NAMES.contains(byte)){
#           else
            if (_TOKEN_NAMES.find(byte) != _TOKEN_NAMES.end()){
#           endif
                cout << _TOKEN_NAMES.at(byte) << " " << byte << " null" << endl;
            }
        }
    }
}
