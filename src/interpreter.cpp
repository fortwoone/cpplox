//
// Created by fortwoone on 02/04/2025.
//

#include "interpreter.hpp"

namespace lox::interpreter{
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