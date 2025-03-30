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
            {'/', "SLASH"},
            {'=', "EQUAL"},
            {'!', "BANG"}
    };

    bool tokenize(const string& file_contents){
        ulong line_count = 1;
        ubyte equal_char_streak = 0;
        bool found_bang = false;
        bool lexical_errors = false;
        for (const auto& byte: file_contents){
#           if __cplusplus >= 202002L
            if (_TOKEN_NAMES.contains(byte)){
#           else
            if (_TOKEN_NAMES.find(byte) != _TOKEN_NAMES.end()){
#           endif
                // region Handling complex operators
                if (byte == '='){
                    if (found_bang){
                        found_bang = false;
                        cout << "BANG_EQUAL != null" << endl;
                    }
                    else if (equal_char_streak < 1){
                        equal_char_streak++;
                    }
                    else{
                        equal_char_streak = 0;
                        cout << "EQUAL_EQUAL == null" << endl;
                    }
                    continue;
                }
                else if (byte == '!'){
                    if (!found_bang){
                        found_bang = true;
                        continue;
                    }
                }
                else{
                    if (equal_char_streak)
                        cout << "EQUAL = null" << endl;
                    else if (found_bang)
                        cout << "BANG ! null" << endl;
                    equal_char_streak = 0;
                    found_bang = false;
                }
                // endregion
                cout << _TOKEN_NAMES.at(byte) << " " << byte << " null" << endl;
            }
            else{
                lexical_errors = true;
                cerr << "[line " << line_count << "] Error: Unexpected character: " << byte << endl;
            }
        }
        if (equal_char_streak){
            cout << "EQUAL = null" << endl;
        }
        return lexical_errors;
    }
}
