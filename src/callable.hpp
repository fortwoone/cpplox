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
        string get_func_name(const shared_ptr<Statement>& func_stmt);

        vector<Token> get_args(const shared_ptr<Statement>& func_stmt);

        ubyte get_arg_count(const shared_ptr<Statement>& func_stmt);
    }
}

namespace lox::interpreter{
    using lox::env::Environment;
    using std::shared_ptr;

    class Interpreter;

    namespace for_ast{
        shared_ptr<Environment> get_current_env(const shared_ptr<Interpreter>& interpreter);

        void set_current_env(const shared_ptr<Interpreter>& interpreter, const shared_ptr<Environment>& env);

        void return_to_previous_env(const shared_ptr<Interpreter>& interpreter);

        void add_nesting_level(const shared_ptr<Interpreter>& interpreter);

        void remove_nesting_level(const shared_ptr<Interpreter>& interpreter);
    }
}

// Actual file declarations (part 1)
namespace lox::callable {
    using lox::ubyte;
    using lox::ast::for_callable::get_args;
    using lox::ast::for_callable::get_arg_count;
    using lox::ast::for_callable::get_func_name;
    using lox::env::Environment;
    using lox::interpreter::for_ast::get_current_env;
    using lox::interpreter::for_ast::set_current_env;
    using lox::interpreter::for_ast::return_to_previous_env;
    using lox::interpreter::for_ast::add_nesting_level;
    using lox::interpreter::for_ast::remove_nesting_level;

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

        [[nodiscard]] virtual Value call(const shared_ptr<Interpreter>& interpreter, const vector<Value> &args) = 0;
        [[nodiscard]] virtual Value call(const shared_ptr<Environment>& env, const vector<Value> &args) = 0;
    };

}

namespace lox::ast::for_callable{
    using lox::callable::EvalResult;
    using lox::interpreter::Interpreter;

    EvalResult exec_func_body(const shared_ptr<Interpreter>& interpreter, const shared_ptr<Statement>& func_stmt);
    EvalResult exec_func_body(const shared_ptr<Environment>& env, const shared_ptr<Statement>& func_stmt);
}

// Putting this here because we need VarValue from lox::callable for future accesses to the environment to work.
namespace lox::env::for_callable{
    using lox::callable::VarValue;

    shared_ptr<Environment> get_child_env(const shared_ptr<Environment>& orig);

    void set_env_member(const shared_ptr<Environment>& func_env, const string& name, VarValue value);
}

// Actual file declarations (part 2)
namespace lox::callable{
    using lox::ast::for_callable::exec_func_body;
    using lox::env::for_callable::get_child_env;
    using lox::env::for_callable::set_env_member;

    bool is_number(const Value& val);

    bool is_boolean(const Value& val);

    bool is_string(const Value& val);

    bool is_callable(const Value& val);

    class LoxFunction: public AbstractLoxCallable{
        shared_ptr<ast::Statement> decl;
        shared_ptr<Environment> closure, child_env;

        public:
            LoxFunction(const shared_ptr<ast::Statement>& decl, const shared_ptr<Environment>& closure);

            [[nodiscard]] string to_string() const{
                return "<fn " + get_func_name(decl) + ">";
            }

            [[nodiscard]] constexpr ubyte arity() const final;

            [[nodiscard]] Value call(const shared_ptr<Interpreter>& interpreter, const vector<Value>& args) final;
            [[nodiscard]] Value call(const shared_ptr<Environment>& env, const vector<Value>& args) final;
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

                [[nodiscard]] Value call(const shared_ptr<Environment>& env, const vector<Value> &args) final;
                [[nodiscard]] Value call(const shared_ptr<Interpreter>& interpreter, const vector<Value>& args) final;
        };
    }
}
