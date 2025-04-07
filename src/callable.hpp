//
// Created by fortwoone on 05/04/2025.
//

#pragma once
#include <cstdint>
#include <ctime>
#include <memory>
#include "utils.hpp"
#include "tokenizer.hpp"

// Forward declarations
namespace lox::env{
    using std::shared_ptr;

    class Environment;

    namespace for_callable{
        shared_ptr<Environment> get_func_env(const shared_ptr<Environment> &globals_env);
    }
}

namespace lox::ast{
    using lox::env::Environment;
    using lox::tokenizer::token::Token;
    using std::shared_ptr;

    class Statement;

    // Functions only used in callable context. Must not be used elsewhere.
    namespace for_callable {
        vector<shared_ptr<Statement>> get_func_body(const shared_ptr<Statement> &func_stmt);

        vector<Token> get_args(const shared_ptr<Statement> &func_stmt);

        ubyte get_arg_count(const shared_ptr<Statement> &func_stmt);

        void exec_stmt_list_as_block(const vector<shared_ptr<Statement>> &stmts, const shared_ptr<Environment> &env);
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
    using lox::ast::for_callable::get_func_body;
    using lox::ast::for_callable::exec_stmt_list_as_block;
    using lox::env::Environment;
    using lox::env::for_callable::get_func_env;
    using lox::interpreter::Interpreter;
    using lox::tokenizer::token::Token;

    using std::make_shared;
    using std::shared_ptr;
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

        [[nodiscard]] virtual constexpr ubyte arity() const = 0;

        [[nodiscard]] virtual Value call(const shared_ptr<Environment> &globals, const vector<Value> &args) const = 0;
    };

}

// Putting this here because we need VarValue from lox::callable for future accesses to the environment to work.
namespace lox::env::for_callable{
    using lox::callable::VarValue;

    void set_env_member(const shared_ptr<Environment>& func_env, const string& name, VarValue value);
}

// Actual file declarations (part 2)
namespace lox::callable{
    using lox::env::for_callable::set_env_member;

    bool is_number(const Value& val);

    bool is_boolean(const Value& val);

    bool is_string(const Value& val);

    bool is_callable(const Value& val);

    class LoxFunction: public AbstractLoxCallable{
        shared_ptr<ast::Statement> decl;

        public:
            LoxFunction(const shared_ptr<ast::Statement>& decl);

            constexpr ubyte arity() const final;

            [[nodiscard]] Value call(const shared_ptr<Environment>& globals, const vector<Value>& args) const final;
    };

    namespace builtins{
        class ClockFunc: public AbstractLoxCallable{
            public:
                [[nodiscard]] constexpr ubyte arity() const final{
                    return 0;
                }
                [[nodiscard]] Value call(const shared_ptr<Environment>& globals, const vector<Value>& args) const final;
        };
    }
}
