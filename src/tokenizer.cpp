//
// Created by fortwoone on 30/03/2025.
//

#include "tokenizer.hpp"


namespace lox::tokenizer{

    namespace literals{
        Literal::Literal(LiteralType lit, const string& orig_val){
            lit_type = lit;
            this->orig_val = orig_val;
        }

        string Literal::get_formatted_value() const{
            return orig_val;
        }

        NullLiteral::NullLiteral(): Literal(LiteralType::NULL_LITERAL, ""){}

        string NullLiteral::get_formatted_value() const{
            return "null";
        }

        StringLiteral::StringLiteral(const string& orig_val): Literal(LiteralType::STRING, orig_val){}

        string StringLiteral::get_formatted_value() const{
            return Literal::get_formatted_value();
        }

        NumberLiteral::NumberLiteral(const string& orig_val): Literal(LiteralType::NUMBER, orig_val){
            val_as_dbl = static_cast<double>(
                stod(orig_val)
            );
            string::size_type dot_pos = orig_val.find(".", 1);
            if (dot_pos == string::npos){
                precision = 1;
            }
            else{
                string decimals = orig_val.substr(dot_pos + 1);
                precision = 0;
                for (size_t i = 0; i < decimals.size(); ++i){
                    if (decimals[i] != '0'){
                        precision = i;
                    }
                }
                precision++;
            }
        }

        string NumberLiteral::get_formatted_value() const{
            ostringstream val_stream;
            val_stream << fixed << setprecision(precision) << val_as_dbl;
            return val_stream.str();
        }
    }

    namespace token{
        // region Constants
        using enum TokenType;

        const unordered_map<char, TokenType> _TOKEN_TYPES{
            {'(', LEFT_PAREN},
            {')', RIGHT_PAREN},
            {'{', LEFT_BRACE},
            {'}', RIGHT_BRACE},
            {'.', DOT},
            {'*', STAR},
            {'-', MINUS},
            {'+', PLUS},
            {';', SEMICOLON},
            {',', COMMA},
            {'/', SLASH},
            {'=', EQUAL},
            {'!', BANG},
            {'<', LESS},
            {'>', GREATER}
        };

        const unordered_map<char, TokenType> _COMPLEX_EQUAL_TOKENS{
            {'=', EQUAL_EQUAL},
            {'!', BANG_EQUAL},
            {'<', LESS_EQUAL},
            {'>', GREATER_EQUAL}
        };

        const unordered_map<string, TokenType> _RESERVED_KW_TOK_TYPES{
            {"and", AND},
            {"class", CLASS},
            {"else", ELSE},
            {"false", FALSE},
            {"for", FOR},
            {"fun", FUN},
            {"if", IF},
            {"nil", NIL},
            {"or", OR},
            {"print", PRINT},
            {"return", RETURN},
            {"super", SUPER},
            {"this", THIS},
            {"true", TRUE},
            {"var", VAR},
            {"while", WHILE}
        };

#           define _STR_NAME_FOR_TOKTP(tok_tp) {tok_tp, #tok_tp}

        const unordered_map<TokenType, string> _TOKENTP_NAMES{
                _STR_NAME_FOR_TOKTP(LEFT_PAREN),
                _STR_NAME_FOR_TOKTP(RIGHT_PAREN),
                _STR_NAME_FOR_TOKTP(LEFT_BRACE),
                _STR_NAME_FOR_TOKTP(RIGHT_BRACE),
                _STR_NAME_FOR_TOKTP(DOT),
                _STR_NAME_FOR_TOKTP(STAR),
                _STR_NAME_FOR_TOKTP(MINUS),
                _STR_NAME_FOR_TOKTP(PLUS),
                _STR_NAME_FOR_TOKTP(SEMICOLON),
                _STR_NAME_FOR_TOKTP(COMMA),
                _STR_NAME_FOR_TOKTP(SLASH),
                _STR_NAME_FOR_TOKTP(EQUAL),
                _STR_NAME_FOR_TOKTP(BANG),
                _STR_NAME_FOR_TOKTP(LESS),
                _STR_NAME_FOR_TOKTP(GREATER),
                _STR_NAME_FOR_TOKTP(EQUAL_EQUAL),
                _STR_NAME_FOR_TOKTP(BANG_EQUAL),
                _STR_NAME_FOR_TOKTP(LESS_EQUAL),
                _STR_NAME_FOR_TOKTP(GREATER_EQUAL),
                _STR_NAME_FOR_TOKTP(STRING),
                _STR_NAME_FOR_TOKTP(NUMBER),
                _STR_NAME_FOR_TOKTP(IDENTIFIER),
                _STR_NAME_FOR_TOKTP(AND),
                _STR_NAME_FOR_TOKTP(CLASS),
                _STR_NAME_FOR_TOKTP(ELSE),
                _STR_NAME_FOR_TOKTP(FALSE),
                _STR_NAME_FOR_TOKTP(FOR),
                _STR_NAME_FOR_TOKTP(FUN),
                _STR_NAME_FOR_TOKTP(IF),
                _STR_NAME_FOR_TOKTP(NIL),
                _STR_NAME_FOR_TOKTP(OR),
                _STR_NAME_FOR_TOKTP(PRINT),
                _STR_NAME_FOR_TOKTP(RETURN),
                _STR_NAME_FOR_TOKTP(SUPER),
                _STR_NAME_FOR_TOKTP(THIS),
                _STR_NAME_FOR_TOKTP(TRUE),
                _STR_NAME_FOR_TOKTP(VAR),
                _STR_NAME_FOR_TOKTP(WHILE),
                {EOF_TOKEN, "EOF"}
        };

#           undef _STR_NAME_FOR_TOKTP
        // endregion

        Token::Token(TokenType token_tp): token_type(token_tp){
            literal = make_shared<literals::NullLiteral>();
            lexeme = "";
        }

        Token::Token(TokenType token_tp, const string& lexeme): token_type(token_tp), lexeme(lexeme){
            literal = make_shared<literals::NullLiteral>();
        }

        Token::Token(TokenType token_tp, const string& lexeme, LiteralType treat_as): token_type(token_tp), lexeme(lexeme){
            switch (treat_as){
                case LiteralType::NUMBER:
                    literal = make_shared<literals::NumberLiteral>(lexeme);
                    break;
                case LiteralType::STRING:
                    literal = make_shared<literals::StringLiteral>(lexeme);
                    break;
                default:
                    literal = make_shared<literals::NullLiteral>();
            }
        }

        void Token::show_in_cli() const{
            bool is_string = (token_type == TokenType::STRING);
            cout << _TOKENTP_NAMES.at(token_type) << (is_string ? " \"" : " ") << lexeme << (is_string ? "\" " : " ") << literal->get_formatted_value() << endl;
        }
    }

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

        const string _IDENTIFIER_CHRS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";

        const auto DEFAULT_PRECISION{cout.precision()};

        const unordered_map<string, string> _RESERVED_KEYWORDS{
            {"and", "AND"},
            {"class", "CLASS"},
            {"else", "ELSE"},
            {"false", "FALSE"},
            {"for", "FOR"},
            {"fun", "FUN"},
            {"if", "IF"},
            {"nil", "NIL"},
            {"or", "OR"},
            {"print", "PRINT"},
            {"return", "RETURN"},
            {"super", "SUPER"},
            {"this", "THIS"},
            {"true", "TRUE"},
            {"var", "VAR"},
            {"while", "WHILE"}
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

        bool is_identifier_char(const char& c){
#if __cplusplus >= 202002L
            return _IDENTIFIER_CHRS.contains(c);
#else
            return _IDENTIFIER_CHRS.find(c) != _IDENTIFIER_CHRS.end();
#endif
        }

        bool is_reserved_kw(const string& literal_str){
#if __cplusplus >= 202002L
            return _RESERVED_KEYWORDS.contains(literal_str);
#else
            return _RESERVED_KEYWORDS.find(literal_str) != _RESERVED_KEYWORDS.end();
#endif
        }

        string get_kw_name(const string& literal_str){
            return _RESERVED_KEYWORDS.at(literal_str);
        }

        void display_number(const string& number){
            string::size_type dot_pos = number.find(".", 1);
            if (dot_pos == string::npos){
                cout << fixed << setprecision(1) << stod(number) << endl;
                reset_precision();
            }
            else{
                string decimals = number.substr(dot_pos + 1);
                size_t actual_precision = 0;
                for (size_t i = 0; i < decimals.size(); ++i){
                    if (decimals[i] != '0'){
                        actual_precision = i;
                    }
                }
                actual_precision++;
                cout << fixed << setprecision(static_cast<int>(actual_precision)) << stod(number) << endl;
            }
            reset_precision();
        }

        void reset_precision(){
            cout << setprecision(DEFAULT_PRECISION);
        }
    }

    vector<token::Token> tokenize(const string& file_contents, bool* contains_errors){
        vector<token::Token> tokens;
        if (file_contents.empty()){
            tokens.emplace_back(token::TokenType::EOF_TOKEN);
            return tokens;
        }

        ulong line_count = 1;
        ulong str_line_start = line_count;
        bool equal_contained_in_op = false;
        bool in_comment = false;
        bool in_string = false;
        bool in_identifier = false;
        bool in_number = false;
        bool got_floating_point_dot = false;
        string literal_str{};
        size_t idx = 0;
        size_t char_count = file_contents.size();
        bool lexical_errors = false;
        for (const auto& byte: file_contents){
            if (in_comment){
                idx++;
                if (byte == '\n'){
                    line_count++;
                    in_comment = false;
                }
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
                    tokens.emplace_back(
                        token::TokenType::STRING,  // token_type
                        literal_str,  // lexeme
                        literals::LiteralType::STRING  // treat_as
                    );
                }
                idx++;
                continue;
            }

            if (in_string){
                literal_str.push_back(byte);
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

            // Check for identifiers.
            if (in_identifier){
                if (priv::is_identifier_char(byte) || priv::is_digit(byte)){
                    // Accept either ASCII, underscores or digits after the first character of an identifier.
                    literal_str.push_back(byte);
                    idx++;
                    continue;
                }
                else{
                    in_identifier = false;
                    if (priv::is_reserved_kw(literal_str)){
                        tokens.emplace_back(
                            token::_RESERVED_KW_TOK_TYPES.at(literal_str),  // token_type
                            literal_str  // lexeme
                        );
                    }
                    else{
                        tokens.emplace_back(
                                token::TokenType::IDENTIFIER,  // token_type
                                literal_str  // lexeme
                        );
                    }
                }
            }
            else{
                if (!in_number){
                    if (priv::is_identifier_char(byte)){
                        in_identifier = true;
                        literal_str.clear();
                        literal_str.push_back(byte);
                        idx++;
                        continue;
                    }
                }
            }

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
                                tokens.emplace_back(
                                    token::TokenType::NUMBER,  // token_type
                                    literal_str,  // lexeme
                                    literals::LiteralType::NUMBER  // treat_as
                                );
                            }
                        }
                        else{  // There are no more characters after the dot.
                            in_number = false;
                            tokens.emplace_back(
                                token::TokenType::NUMBER,  // token_type
                                literal_str,  // lexeme
                                literals::LiteralType::NUMBER  // treat_as
                            );
                        }
                    }
                    else{  // It's the second dot hit while reading the literal.
                        in_number = false;
                        tokens.emplace_back(
                            token::TokenType::NUMBER,  // token_type
                            literal_str,  // lexeme
                            literals::LiteralType::NUMBER  // treat_as
                        );
                    }
                }
                else if (!priv::is_digit(byte)){  // The current character isn't a digit nor a dot.
                    in_number = false;
                    tokens.emplace_back(
                        token::TokenType::NUMBER,  // token_type
                        literal_str,  // lexeme
                        literals::LiteralType::NUMBER  // treat_as
                    );
                }
                else{  // The current character is a digit.
                    literal_str.push_back(byte);
                    idx++;
                    continue;
                }
            }
            else{
                if (!in_string && !in_identifier){  // Not reading a string currently.
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
                idx++;
                if (byte == '\n'){
                    line_count++;
                }
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

                // Handling complex operators
                if (priv::is_complex_token(byte)){
                    if (idx + 1 < char_count && file_contents[idx + 1] == '='){
                        // If the operator's followed by an '=', ignore it when looping on it next.
                        equal_contained_in_op = true;
                        string _complex_lexeme;
                        _complex_lexeme.reserve(2);
                        _complex_lexeme.push_back(byte);
                        _complex_lexeme.push_back('=');
                        tokens.emplace_back(
                                token::_COMPLEX_EQUAL_TOKENS.at(byte),  // token_type
                                _complex_lexeme  // lexeme
                        );
                        idx++;
                        continue;
                    }
                }

                tokens.emplace_back(
                        token::_TOKEN_TYPES.at(byte),  // token_type
                        string(1, byte)  // lexeme (creating a string from the single character)
                );
            }
            else{
                // Repeating the identifier checks to look for a possible identifier start immediately after a literal.
                if (priv::is_identifier_char(byte)){
                    in_identifier = true;
                    literal_str.clear();
                    literal_str.push_back(byte);
                    idx++;
                    continue;
                }
                // Unrecognised token character. Show an error has occurred.
                lexical_errors = true;
                cerr << "[line " << line_count << "] Error: Unexpected character: " << byte << endl;
            }
            idx++;
        }

        if (in_number){
            in_number = false;
            tokens.emplace_back(
                token::TokenType::NUMBER,  // token_type
                literal_str,  // lexeme
                literals::LiteralType::NUMBER  // treat_as
            );
        }

        if (in_identifier){
            in_identifier = false;
            if (priv::is_reserved_kw(literal_str)){
                tokens.emplace_back(
                        token::_RESERVED_KW_TOK_TYPES.at(literal_str),  // token_type
                        literal_str  // lexeme
                );
            }
            else{
                tokens.emplace_back(
                        token::TokenType::IDENTIFIER,  // token_type
                        literal_str  // lexeme
                );
            }
        }

        if (in_string){
            // A string literal was not terminated (still reading a string upon reaching the end of the file).
            cerr << "[line " << str_line_start << "] Error: Unterminated string." << endl;
            lexical_errors = true;
        }
        tokens.emplace_back(token::TokenType::EOF_TOKEN);
        (*contains_errors) = lexical_errors;
        return tokens;
    }
}

