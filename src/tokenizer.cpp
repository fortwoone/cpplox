//
// Created by fortwoone on 30/03/2025.
//

#include "tokenizer.hpp"

namespace tokenizer{
    namespace priv{
        // region Constants
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

        const unordered_set<char> _DIGITS{
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
        };
        // endregion

        bool is_token(const char& c){
#if __cplusplus >= 202002L
            return _TOKEN_NAMES.contains(c);
#else
            return _TOKEN_NAMES.find(c) != _TOKEN_NAMES.end();
#endif
        }

        string get_token_name(const char& c){
            return _TOKEN_NAMES.at(c);
        }

        bool is_complex_token(const char& c){
#if __cplusplus >= 202002L
            return _COMPLEX_TOKENS.contains(c);
#else
            return _COMPLEX_TOKENS.find(c) != _COMPLEX_TOKENS.end();
#endif
        }

        bool is_ignore_char(const char& c){
#if __cplusplus >= 202002L
            return _IGNORE_CHARS.contains(c);
#else
            return _IGNORE_CHARS.find(c) != _IGNORE_CHARS.end();
#endif
        }

        bool is_digit(const char& c){
#if __cplusplus >= 202002L
            return _DIGITS.contains(c);
#else
            return _DIGITS.find(c) != _DIGITS.end();
#endif
        }
    }

    bool tokenize(const string& file_contents){
        ulong line_count = 1;
        ulong str_line_start = line_count;
        bool equal_contained_in_op = false;
        bool in_comment = false;
        bool in_string = false;
        bool in_number = false;
        bool got_floating_point_dot = false;
        string literal_str{};
        size_t idx = 0;
        size_t char_count = file_contents.size();
        bool lexical_errors = false;
        for (const auto& byte: file_contents){
            // Check for number literals.
            if (in_number){
                if (byte == '.'){  // Hit a dot.
                    if (!got_floating_point_dot){  // It's the first dot in the literal.
                        if (idx + 1 < char_count){  // There are other characters afterwards.
                            if (priv::is_digit(file_contents[idx + 1])){  // The immediate next one is a digit.
                                got_floating_point_dot = true;
                                literal_str.push_back(byte);
                                idx++;
                                continue;
                            }
                            else{  // There are no digits after this dot.
                                in_number = false;
                                cout << "NUMBER " << literal_str << " " << stod(literal_str) << endl;
                            }
                        }
                        else{  // There are no more characters after the dot.
                            in_number = false;
                            cout << "NUMBER " << literal_str << " " << stod(literal_str) << endl;
                        }
                    }
                    else{  // It's the second dot hit while reading the literal.
                        in_number = false;
                        cout << "NUMBER " << literal_str << " " << stod(literal_str) << endl;
                    }
                }
                else if (!priv::is_digit(byte)){  // The current character isn't a digit nor a dot.
                    in_number = false;
                    cout << "NUMBER " << literal_str << " " << stod(literal_str) << endl;
                }
                else{  // The current character is a digit.
                    literal_str.push_back(byte);
                    idx++;
                    continue;
                }
            }
            else{
                if (!in_string){  // Not reading a string currently.
                    if (priv::is_digit(byte)){  // The current character is a digit.
                        in_number = true;
                        got_floating_point_dot = false;
                        literal_str.clear();
                        literal_str.push_back(byte);
                        idx++;
                        continue;
                    }
                }
            }

            // Check for tabs or whitespace characters. If the current byte is either a space or a tab, ignore it.
            if (priv::is_ignore_char(byte)){
                if (in_string){
                    literal_str.push_back(byte);
                }
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
                idx++;
                continue;  // Ignore characters until next line if in a comment.
            }

            if (byte == '"'){
                // Start reading a string literal when reaching a double quote,
                // or finish reading it if a string literal was already being read.
                if (!in_string){
                    in_string = true;
                    literal_str.clear();
                    str_line_start = line_count;
                }
                else{
                    in_string = false;
                    cout << "STRING \"" << literal_str << "\" " << literal_str << endl;
                }
                idx++;
                continue;
            }

            if (in_string){
                literal_str.push_back(byte);
                idx++;
                continue;
            }

            if (priv::is_token(byte)){
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
                if (priv::is_complex_token(byte)){
                    if (idx + 1 < char_count && file_contents[idx + 1] == '='){
                        // If the operator's followed by an '=', ignore it when looping on it next.
                        equal_contained_in_op = true;
                        cout << priv::get_token_name(byte) << "_EQUAL " << byte << "= null" << endl;
                        idx++;
                        continue;
                    }
                }
                cout << priv::get_token_name(byte) << " " << byte << " null" << endl;
            }
            else{
                // Unrecognised token character. Show an error has occurred.
                lexical_errors = true;
                cerr << "[line " << line_count << "] Error: Unexpected character: " << byte << endl;
            }
            idx++;
        }

        if (in_number){
            in_number = false;
            cout << "NUMBER " << literal_str << " " << stod(literal_str) << endl;
        }

        if (in_string){
            // A string literal was not terminated (still reading a string upon reaching the end of the file).
            cerr << "[line " << str_line_start << "] Error: Unterminated string." << endl;
            lexical_errors = true;
        }
        return lexical_errors;
    }
}
