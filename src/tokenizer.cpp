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
            {'!', "BANG"},
            {'<', "LESS"},
            {'>', "GREATER"}
    };

    const unordered_set<char> _COMPLEX_TOKENS{
        '=', '!', '<', '>'
    };

    const unordered_set<char> _IGNORE_CHARS{
        '\t', ' ', '\n'
    };

    bool tokenize(const string& file_contents){
        ulong line_count = 1;
        bool equal_contained_in_op = false;
        bool in_comment = false;
        size_t idx = 0;
        size_t char_count = file_contents.size();
        bool lexical_errors = false;
        for (const auto& byte: file_contents){

            // Check for tabs or whitespace characters. If the current byte is either a space or a tab, ignore it.
#           if __cplusplus >= 202002L
            if (_IGNORE_CHARS.contains(byte)){
#           else
                if (_IGNORE_CHARS.find(byte) != _IGNORE_CHARS.end()){
#           endif
                idx++;
                if (byte == '\n'){
                    line_count++;
                    if (in_comment){
                        // Comment state and found a line feed: resume parsing after the next character.
                        in_comment = false;
                    }
                }
                continue;
            }

            if (in_comment){
                continue;  // Ignore characters until next line.
            }

#           if __cplusplus >= 202002L
            if (_TOKEN_NAMES.contains(byte)){
#           else
            if (_TOKEN_NAMES.find(byte) != _TOKEN_NAMES.end()){
#           endif
                // Skip tokenising the current byte if it's an equal and one of the tokens in _COMPLEX_TOKENS
                // was found as the immediate previous character.
                if (equal_contained_in_op){
                    equal_contained_in_op = false;
                    idx++;
                    continue;
                }

                // Check for comment start. If two slashes are found, immediately stop tokenising.
                if (byte == '/'){
                    if (idx + 1 < char_count && file_contents[idx + 1] == '/'){
                        // Comment start. Stop parsing and ignore all characters until the next line feed.
                        in_comment = true;
                        idx++;
                        continue;
                    }
                }

                // Handling complex operators
#               if __cplusplus >= 202002L
                if (_COMPLEX_TOKENS.contains(byte)){
#               else
                if (_COMPLEX_TOKENS.find(byte) != _COMPLEX_TOKENS.end()){
#               endif
                    if (idx + 1 < char_count && file_contents[idx + 1] == '='){
                        // If the operator's followed by an '=', ignore it when looping on it next.
                        equal_contained_in_op = true;
                        cout << _TOKEN_NAMES.at(byte) << "_EQUAL " << byte << "= null" << endl;
                        idx++;
                        continue;
                    }
                }
                cout << _TOKEN_NAMES.at(byte) << " " << byte << " null" << endl;
            }
            else{
                lexical_errors = true;
                cerr << "[line " << line_count << "] Error: Unexpected character: " << byte << endl;
            }
            idx++;
        }
        return lexical_errors;
    }
}
