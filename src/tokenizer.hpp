//
// Created by fortwoone on 30/03/2025.
//

#pragma once
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace tokenizer{
    using ulong = uint64_t;
    using std::any_of;
    using std::cout;
    using std::cerr;
    using std::endl;
    using std::exit;
    using std::fixed;
    using std::setprecision;
    using std::stoi;
    using std::stod;
    using std::strcmp;
    using std::string;
    using std::unordered_map;
    using std::unordered_set;

    namespace priv{
        bool is_token(const char& c);
        string get_token_name(const char& c);
        bool is_complex_token(const char& c);
        bool is_ignore_char(const char& c);
        bool is_digit(const char& c);
        bool is_identifier_char(const char& c);
        bool is_reserved_kw(const string& literal_str);
        string get_kw_name(const string& kw_name);
        void display_number(const string& number);
        void reset_precision();
    }

    bool tokenize(const string& file_contents);
}
