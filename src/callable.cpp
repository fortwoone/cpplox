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

    LoxFunction::LoxFunction(const shared_ptr<ast::Statement>& decl,const shared_ptr<Environment>& closure): AbstractLoxCallable(), decl(decl), closure(closure){
        child_env = get_child_env(closure);
    }

    constexpr ubyte LoxFunction::arity() const{
        return get_arg_count(decl);
    }

    Value LoxFunction::call(const shared_ptr<Environment>& env, const vector<Value>& args){
        vector<Token> decl_args = get_args(decl);

        for (ubyte i = 0; i < get_arg_count(decl); ++i){
            set_env_member(child_env, decl_args.at(i).get_lexeme(), args.at(i));
        }

        Value ret_val = exec_func_body(child_env, decl);

        return ret_val;
    }

    Value LoxFunction::call(const shared_ptr<Interpreter>& interpreter, const vector<Value>& args){
        vector<Token> decl_args = get_args(decl);

        for (ubyte i = 0; i < get_arg_count(decl); ++i){
            set_env_member(child_env, decl_args.at(i).get_lexeme(), args.at(i));
        }

        set_current_env(interpreter, child_env);
        Value ret_val = exec_func_body(interpreter, decl);
        return_to_previous_env(interpreter);
        child_env = get_child_env(closure);

        return ret_val;
    }

    namespace builtins{
        Value ClockFunc::call(const shared_ptr<Environment>& interpreter, const vector<Value> &args){
            return (double)time(nullptr);
        }

        Value ClockFunc::call(const shared_ptr<Interpreter>& interpreter, const vector<Value> &args){
            return (double)time(nullptr);
        }
    }
}