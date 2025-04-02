//
// Created by fortwoone on 02/04/2025.
//

#include "interpreter.hpp"

namespace lox::interpreter{
    Interpreter::Interpreter(const string& file_contents){
        bool contains_errors = false;
        vector<Token> tokenised = tokenize(file_contents, &contains_errors);
        if (contains_errors){
            throw parse_error(65, "Error while parsing file (tokenising stage)\0");
        }

        Parser parser = Parser(tokenised);
        statements = parser.parse();
    }

    Interpreter::Interpreter(const vector<StmtPtr>& statements): statements(statements){}

    void Interpreter::run(){
        try{
            for (const auto& stmt: statements){
                stmt->execute();
            }
        }
        catch (const exception& exc){
            throw runtime_error(exc.what());
        }
    }

    void run(const string& file_contents){
        auto interpreter = Interpreter(file_contents);
        interpreter.run();
    }
}