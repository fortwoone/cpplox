//
// Created by fortwoone on 30/03/2025.
//

#pragma once
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace tokenizer{
    using ubyte = uint8_t;
    using ulong = uint64_t;
    using std::cout;
    using std::cerr;
    using std::endl;
    using std::exit;
    using std::string;
    using std::unordered_map;
    using std::unordered_set;

    bool tokenize(const string& file_contents);
}
