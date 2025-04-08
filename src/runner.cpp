//
// Created by fortwoone on 08/04/2025.
//

#include "runner.hpp"

namespace lox::runner{
    // No need to constantly check for errors, since exceptions are thrown if parsing, running or resolving fail.
    void run(const string& file_contents){
        bool contains_errors = false;
        Parser parser = Parser(tokenize(file_contents, &contains_errors));

        vector<shared_ptr<ast::Statement>> statements = parser.parse();
        shared_ptr<Interpreter> interpreter = make_shared<Interpreter>(statements);

        shared_ptr<Resolver> resolver = make_shared<Resolver>(interpreter);
        resolver->resolve(statements);

        interpreter->run();
    }
}
