//
// Created by fortwoone on 06/04/2025.
//

#pragma once
#include <stdexcept>
#include "callable.hpp"
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

    namespace priv{
        using lox::callable::Value;
    }

    class return_exc: public exception{
        priv::Value ret_val;

        public:
            return_exc(priv::Value ret_val): exception(), ret_val(ret_val){}

            [[nodiscard]] priv::Value get_returned_val() const{
                return ret_val;
            }
    };

    class resolve_error: public exception{
        const char* message;

        public:
            resolve_error(const char* msg): message(msg){}

            [[nodiscard]] const char* what() const noexcept override{
                return message;
            }
    };
}
