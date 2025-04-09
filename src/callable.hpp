//
// Created by fortwoone on 05/04/2025.
//

#pragma once
#include <cmath>
#include <cstdint>
#include <ctime>
#include <memory>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include "utils.hpp"
#include "tokenizer.hpp"

// Forward declarations (part 1)
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

namespace lox::inst{
    using std::shared_ptr;

    class LoxInstance;
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
    using namespace lox::ast::for_callable;
    using lox::env::Environment;
    using lox::inst::LoxInstance;
    using namespace lox::interpreter::for_ast;

    using lox::interpreter::Interpreter;
    using lox::tokenizer::token::Token;

    using std::cos;
    using std::dynamic_pointer_cast;
    using std::enable_shared_from_this;
    using std::make_shared;
    using std::runtime_error;
    using std::shared_ptr;
    using std::sin;
    using std::stack;
    using std::string;
    using std::time;
    using std::time_t;

    class AbstractLoxCallable;

    using CallablePtr = shared_ptr<AbstractLoxCallable>;
    using InstancePtr = shared_ptr<LoxInstance>;

    using Value = variant<double, bool, string, CallablePtr, InstancePtr>;
    using VarValue = Value;
    using EvalResult = Value;

    class AbstractLoxCallable: public enable_shared_from_this<AbstractLoxCallable>{
    public:
        virtual ~AbstractLoxCallable() = default;

        [[nodiscard]] virtual string to_string() const = 0;

        [[nodiscard]] virtual constexpr ubyte arity() const = 0;

        [[nodiscard]] virtual Value call(const shared_ptr<Interpreter>& interpreter, const vector<Value>& args) = 0;
        [[nodiscard]] virtual Value call(const shared_ptr<Environment>& env, const vector<Value>& args) = 0;
    };

}

// Forward declarations (part 2)
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

    VarValue value_of_this(const shared_ptr<Environment>& orig);

    void set_env_member(const shared_ptr<Environment>& func_env, const string& name, VarValue value);
}

// Actual file declarations (part 2)
namespace lox::callable {
    using lox::ast::for_callable::exec_func_body;
    using lox::env::for_callable::get_child_env;
    using lox::env::for_callable::set_env_member;
    using lox::env::for_callable::value_of_this;

    bool is_number(const Value &val);

    bool is_boolean(const Value &val);

    bool is_string(const Value &val);

    bool is_callable(const Value &val);

    bool is_cls_inst(const Value &val);

    class LoxFunction : public AbstractLoxCallable {
        shared_ptr<ast::Statement> decl;
        shared_ptr<Environment> closure, child_env;
        stack<shared_ptr<Environment>> prev_children;
        bool is_init;

    public:
        LoxFunction(const shared_ptr<ast::Statement>& decl, const shared_ptr<Environment>& closure, bool is_initialiser);

        [[nodiscard]] string to_string() const final {
            return "<fn " + get_func_name(decl) + ">";
        }

        shared_ptr<LoxFunction> bind(const InstancePtr& inst);

        [[nodiscard]] constexpr ubyte arity() const final;

        [[nodiscard]] Value call(const shared_ptr<Interpreter>& interpreter, const vector<Value>& args) final;

        [[nodiscard]] Value call(const shared_ptr<Environment>& env, const vector<Value>& args) final;
    };

    class LoxClass;
}

// Forward declarations (part 3)
namespace lox::inst::for_callable{
    using lox::callable::LoxClass;

    shared_ptr<LoxInstance> create_inst(const shared_ptr<LoxClass>& cls);
}

// Actual file declarations (part 3)
namespace lox::callable{
    using lox::inst::for_callable::create_inst;

    using std::unordered_map;
    using MethodMap = unordered_map<string, shared_ptr<LoxFunction>>;

    class LoxClass: public AbstractLoxCallable{
        string name;
        shared_ptr<LoxClass> superclass;
        MethodMap methods;
        ubyte init_arity;

        public:
            explicit LoxClass(const string& cls_name, const shared_ptr<LoxClass>& superclass, const MethodMap& meths);

            shared_ptr<LoxFunction> find_meth(const string& meth_name);

            [[nodiscard]] constexpr ubyte arity() const final;

            [[nodiscard]] string to_string() const final{
                return name;
            }

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

                [[nodiscard]] Value call(const shared_ptr<Environment>& env, const vector<Value>& args) final;
                [[nodiscard]] Value call(const shared_ptr<Interpreter>& interpreter, const vector<Value>& args) final;
        };

        class CosFunc: public AbstractLoxCallable{
            public:
                [[nodiscard]] string to_string() const final{
                    return "<fn cos>";
                }

                [[nodiscard]] constexpr ubyte arity() const final{
                    return 1;
                }

                [[nodiscard]] Value call(const shared_ptr<Environment>& env, const vector<Value>& args) final;
                [[nodiscard]] Value call(const shared_ptr<Interpreter>& interpreter, const vector<Value>& args) final;
        };

        class SinFunc: public AbstractLoxCallable{
            public:
                [[nodiscard]] string to_string() const final{
                    return "<fn sin>";
                }

                [[nodiscard]] constexpr ubyte arity() const final{
                    return 1;
                }

                [[nodiscard]] Value call(const shared_ptr<Environment>& env, const vector<Value>& args) final;
                [[nodiscard]] Value call(const shared_ptr<Interpreter>& interpreter, const vector<Value>& args) final;
        };
    }
}
