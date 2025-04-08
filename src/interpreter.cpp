//
// Created by fortwoone on 02/04/2025.
//

#include "interpreter.hpp"

namespace lox::interpreter{
    VarValue Interpreter::look_up_variable(const string& name, const ExprPtr& expr){
        if (locals.contains(expr)){
            return env->get_at(locals.at(expr), name);
        }
        return globals->get(name);
    }

    void Interpreter::assign_var(const string& name, const ExprPtr& expr){
        VarValue val = expr->evaluate(shared_from_this());
        if (locals.contains(expr)){
            env->assign_at(
                locals.at(expr),
                name,
                val
            );
        }
        else{
            globals->assign(name, val);
        }
    }

    void Interpreter::define_builtins(){
        globals->set("clock", make_shared<builtins::ClockFunc>());
    }

    Interpreter::Interpreter(const string& file_contents){
        env = make_shared<Environment>();
        globals = env;
        define_builtins();
        bool contains_errors = false;

        Parser parser = Parser(tokenize(file_contents, &contains_errors));
        statements = parser.parse();
    }

    Interpreter::Interpreter(const vector<StmtPtr>& statements): statements(statements){
        env = make_shared<Environment>();
        globals = env;
        define_builtins();
    }

    void Interpreter::run(){
        shared_ptr<Interpreter> shared = shared_from_this();
        for (const auto& stmt: statements){
            stmt->execute(shared);
        }
    }

    void run(const string& file_contents){
        shared_ptr<Interpreter> interpreter = make_shared<Interpreter>(file_contents);
        interpreter->run();
    }

    namespace for_ast{
        VarValue look_up_var(const shared_ptr<Interpreter>& interpreter, const string& name, const shared_ptr<ast::Expr>& expr){
            return interpreter->look_up_variable(name, expr);
        }

        void assign_var(const shared_ptr<Interpreter>& interpreter, const string& name, const ExprPtr& expr){
            interpreter->assign_var(name, expr);
        }

        shared_ptr<Environment> get_current_env(const shared_ptr<Interpreter>& interpreter){
            return interpreter->get_current_env();
        }

        void add_nesting_level(const shared_ptr<Interpreter>& interpreter){
            interpreter->add_nesting_level();
        }

        void remove_nesting_level(const shared_ptr<Interpreter>& interpreter){
            interpreter->remove_nesting_level();
        }
    }
}