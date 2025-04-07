//
// Created by fortwoone on 05/04/2025.
//

#include "callable.hpp"

namespace lox::callable{
    bool is_number(const Value& val){
        return holds_alternative<double>(val);
    }

    bool is_boolean(const Value& val){
        return holds_alternative<bool>(val);
    }

    bool is_string(const Value& val){
        return holds_alternative<string>(val);
    }

    bool is_callable(const Value& val){
        return holds_alternative<CallablePtr>(val);
    }

    LoxFunction::LoxFunction(const shared_ptr<ast::Statement>& decl): AbstractLoxCallable(), decl(decl){}

    constexpr ubyte LoxFunction::arity() const{
        return get_arg_count(decl);
    }

    Value LoxFunction::call(const shared_ptr<Interpreter>& interpreter, const vector<Value>& args) const{
        add_nesting_level(interpreter);
        shared_ptr<Environment> func_env = get_current_env(interpreter);
        vector<Token> decl_args = get_args(decl);

        for (ubyte i = 0; i < get_arg_count(decl); ++i){
            set_env_member(func_env, decl_args.at(i).get_lexeme(), args.at(i));
        }

        Value ret_val = exec_func_body(interpreter, decl);
        remove_nesting_level(interpreter);

        return ret_val;
    }

    namespace builtins{
        Value ClockFunc::call(const shared_ptr<Interpreter>& interpreter, const vector<Value> &args) const {
            return (double)time(nullptr);
        }
    }
}