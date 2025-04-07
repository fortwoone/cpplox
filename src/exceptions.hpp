//
// Created by fortwoone on 06/04/2025.
//

#pragma once
#include <stdexcept>
#include "utils.hpp"

namespace lox{
    using std::exception;

    class parse_error: public exception{
        ubyte return_code;
        const char* message;

    public:
        parse_error(ubyte return_code, const char* message): return_code(return_code), message(message){}
        [[nodiscard]] const char* what() const noexcept override{
            return message;
        }
        [[nodiscard]] ubyte get_return_code() const noexcept{
            return return_code;
        }
    };
}
