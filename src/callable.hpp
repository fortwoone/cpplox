//
// Created by fortwoone on 05/04/2025.
//

#pragma once
#include <cstdint>
#include <ctime>
#include <memory>
#include <string>
#include "utils.hpp"
#include "tokenizer.hpp"

// Forward declarations
namespace lox::env{
    using std::shared_ptr;

    class Environment;
}

namespace lox::ast{
    using lox::env::Environment;
    using lox::tokenizer::token::Token;
    using std::shared_ptr;
    using std::string;

    class Statement;

    // Functions only used in callable context. Must not be used elsewhere.
    namespace for_callable {
        shared_ptr<Environment> get_func_env(const shared_ptr<Statement>& globals_env);

        string get_func_name(const shared_ptr<Statement>& func_stmt);

        vector<Token> get_args(const shared_ptr<Statement>& func_stmt);

        ubyte get_arg_count(const shared_ptr<Statement>& func_stmt);
    }
}

namespace lox::interpreter{
    class Interpreter;
}

// Actual file declarations (part 1)
namespace lox::callable {
    using lox::ubyte;
    using lox::ast::for_callable::get_args;
    using lox::ast::for_callable::get_arg_count;
    using lox::ast::for_callable::get_func_env;
    using lox::ast::for_callable::get_func_name;
    using lox::env::Environment;
    using lox::interpreter::Interpreter;
    using lox::tokenizer::token::Token;

    using std::make_shared;
    using std::shared_ptr;
    using std::string;
    using std::time;
    using std::time_t;

    class AbstractLoxCallable;

    using CallablePtr = shared_ptr<AbstractLoxCallable>;

    using Value = variant<double, bool, string, CallablePtr>;
    using VarValue = Value;
    using EvalResult = Value;

    class AbstractLoxCallable {
    public:
        virtual ~AbstractLoxCallable() = default;

        [[nodiscard]] virtual string to_string() const = 0;

        [[nodiscard]] virtual constexpr ubyte arity() const = 0;

        [[nodiscard]] virtual Value call(const vector<Value> &args) const = 0;
    };

}

namespace lox::ast::for_callable{
    using lox::callable::EvalResult;

    EvalResult exec_func_body(const shared_ptr<Statement>& func_stmt);
}

// Putting this here because we need VarValue from lox::callable for future accesses to the environment to work.
namespace lox::env::for_callable{
    using lox::callable::VarValue;

    void set_env_member(const shared_ptr<Environment>& func_env, const string& name, VarValue value);
}

// Actual file declarations (part 2)
namespace lox::callable{
    using lox::ast::for_callable::exec_func_body;
    using lox::env::for_callable::set_env_member;

    bool is_number(const Value& val);

    bool is_boolean(const Value& val);

    bool is_string(const Value& val);

    bool is_callable(const Value& val);

    class LoxFunction: public AbstractLoxCallable{
        shared_ptr<ast::Statement> decl;

        public:
            LoxFunction(const shared_ptr<ast::Statement>& decl);

            [[nodiscard]] string to_string() const{
                return "<fn " + get_func_name(decl) + ">";
            }

            [[nodiscard]] constexpr ubyte arity() const final;

            [[nodiscard]] Value call(const vector<Value>& args) const final;
    };

    namespace builtins{
        class ClockFunc: public AbstractLoxCallable{
            public:
                [[nodiscard]] string to_string() const final{
                    return "<fn clock>";
                }

                [[nodiscard]] constexpr ubyte arity() const final{
                    return 0;
                }
                [[nodiscard]] Value call(const vector<Value>& args) const final;
        };
    }
}
