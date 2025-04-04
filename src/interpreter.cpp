//
// Created by fortwoone on 02/04/2025.
//

#include "interpreter.hpp"

namespace lox::interpreter{
    Interpreter::Interpreter(const string& file_contents){
        env = make_shared<Environment>();
        bool contains_errors = false;

        Parser parser = Parser(tokenize(file_contents, &contains_errors), env);
        statements = parser.parse();
    }

    Interpreter::Interpreter(const vector<StmtPtr>& statements): statements(statements){}

    void Interpreter::execute_var_statement(const shared_ptr<VariableStatement>& var_stmt){
        VarValue val = "nil";
        if (var_stmt->get_initialiser() != nullptr){
            val = var_stmt->get_initialiser()->evaluate();
        }

        env->set(var_stmt->get_name(), val);
    }

    void Interpreter::run(){
        for (const auto& stmt: statements){
            auto as_var_stmt = dynamic_pointer_cast<VariableStatement>(stmt);
            if (as_var_stmt != nullptr){
                execute_var_statement(as_var_stmt);
                continue;
            }
            stmt->execute();
        }
    }

    void run(const string& file_contents){
        auto interpreter = Interpreter(file_contents);
        interpreter.run();
    }
}