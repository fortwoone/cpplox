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

    bool is_cls_inst(const Value& val){
        return holds_alternative<InstancePtr>(val);
    }

    // region LoxFunction
    LoxFunction::LoxFunction(const shared_ptr<ast::Statement>& decl,const shared_ptr<Environment>& closure, bool is_initialiser)
    : AbstractLoxCallable(), decl(decl), closure(closure), is_init(is_initialiser){
        child_env = get_child_env(closure);
    }

    shared_ptr<LoxFunction> LoxFunction::bind(const InstancePtr& inst){
        auto bound_closure = get_child_env(closure);
        set_env_member(bound_closure, "this", inst);
        return make_shared<LoxFunction>(decl, bound_closure, is_init);
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
        if (is_init){
            return value_of_this(closure);
        }

        return ret_val;
    }

    Value LoxFunction::call(const shared_ptr<Interpreter>& interpreter, const vector<Value>& args){
        prev_children.push(child_env);
        child_env = get_child_env(closure);
        vector<Token> decl_args = get_args(decl);

        for (ubyte i = 0; i < get_arg_count(decl); ++i){
            set_env_member(child_env, decl_args.at(i).get_lexeme(), args.at(i));
        }

        set_current_env(interpreter, child_env);
        Value ret_val = exec_func_body(interpreter, decl);
        return_to_previous_env(interpreter);
        child_env = prev_children.top();
        prev_children.pop();
        if (is_init){
            return value_of_this(closure);
        }

        return ret_val;
    }
    // endregion

    // region LoxClass
    LoxClass::LoxClass(const string& cls_name, const shared_ptr<LoxClass>& superclass, const MethodMap& meths)
    : name(cls_name), superclass(superclass), methods(meths){
        auto initialiser = find_meth("init");
        if (initialiser != nullptr){
            init_arity = initialiser->arity();
        }
        else{
            init_arity = 0;
        }
    }

    shared_ptr<LoxFunction> LoxClass::find_meth(const string& meth_name){
        if (methods.contains(meth_name)){
            return methods.at(meth_name);
        }

        if (superclass != nullptr)
            return superclass->find_meth(meth_name);

        return nullptr;
    }

    constexpr ubyte LoxClass::arity() const{
        return init_arity;
    }

    Value LoxClass::call(const shared_ptr<Environment>& env, const vector<Value>& args){
        auto shared = dynamic_pointer_cast<LoxClass>(shared_from_this());
        auto inst = create_inst(shared);
        shared_ptr<LoxFunction> initialiser = find_meth("init");
        if (initialiser != nullptr){
            VarValue val = initialiser->bind(inst)->call(env, args);
        }
        return inst;
    }

    Value LoxClass::call(const shared_ptr<Interpreter>& interpreter, const vector<Value>& args){
        auto shared = dynamic_pointer_cast<LoxClass>(shared_from_this());
        auto inst = create_inst(shared);
        shared_ptr<LoxFunction> initialiser = find_meth("init");
        if (initialiser != nullptr){
            VarValue val = initialiser->bind(inst)->call(interpreter, args);
        }
        return inst;
    }

    // endregion

    namespace builtins{
        Value ClockFunc::call(const shared_ptr<Environment>& interpreter, const vector<Value>& args){
            return (double)time(nullptr);
        }

        Value ClockFunc::call(const shared_ptr<Interpreter>& interpreter, const vector<Value>& args){
            return (double)time(nullptr);
        }

        Value SinFunc::call(const shared_ptr<Environment>& env, const vector<Value>& args){
            EvalResult nb = args.at(0);
            if (!is_number(nb)){
                throw runtime_error("A number is needed when calling sin.");
            }
            return sin(get<double>(nb));
        }

        Value SinFunc::call(const shared_ptr<Interpreter>& interpreter, const vector<Value>& args){
            EvalResult nb = args.at(0);
            if (!is_number(nb)){
                throw runtime_error("A number is needed when calling sin.");
            }
            return sin(get<double>(nb));
        }

        Value CosFunc::call(const shared_ptr<Environment>& env, const vector<Value>& args){
            EvalResult nb = args.at(0);
            if (!is_number(nb)){
                throw runtime_error("A number is needed when calling cos.");
            }
            return cos(get<double>(nb));
        }

        Value CosFunc::call(const shared_ptr<Interpreter>& interpreter, const vector<Value>& args){
            EvalResult nb = args.at(0);
            if (!is_number(nb)){
                throw runtime_error("A number is needed when calling cos.");
            }
            return cos(get<double>(nb));
        }
    }
}