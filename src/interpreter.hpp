//
// Created by fortwoone on 02/04/2025.
//

#pragma once
#include "tokenizer.hpp"
#include "parser.hpp"
#include "env.hpp"
#include "exceptions.hpp"
#include "callable.hpp"
#include "ast.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace lox::interpreter{
    namespace builtins = lox::callable::builtins;
    using lox::env::Environment;
    using lox::callable::VarValue;
    using lox::parser::ExprPtr;
    using lox::parser::Parser;
    using lox::parser::StmtPtr;
    using lox::tokenizer::token::Token;
    using lox::tokenizer::tokenize;

    using std::dynamic_pointer_cast;
    using std::enable_shared_from_this;
    using std::exception;
    using std::make_shared;
    using std::runtime_error;
    using std::shared_ptr;
    using std::string;
    using std::unordered_map;
    using std::vector;

    class Interpreter: public enable_shared_from_this<Interpreter>{
        vector<StmtPtr> statements;
        shared_ptr<Environment> globals;
        shared_ptr<Environment> env;
        unordered_map<ExprPtr, size_t> locals;

        friend VarValue for_ast::look_up_var(const shared_ptr<Interpreter>& interpreter, const string& name, const shared_ptr<ast::Expr>& expr);
        friend void for_ast::assign_var(const shared_ptr<Interpreter>& interpreter, const string& name, const shared_ptr<ast::Expr>& expr);

        VarValue look_up_variable(const string& name, const ExprPtr& expr);
        void assign_var(const string& name, const ExprPtr& expr);
        void define_builtins();

        public:
            explicit Interpreter(const string& file_contents);
            explicit Interpreter(const vector<StmtPtr>& statements);

            [[nodiscard]] shared_ptr<Environment> get_globals() const{
                return globals;
            }

            [[nodiscard]] shared_ptr<Environment> get_current_env() const{
                return env;
            }

            void resolve(const ExprPtr& expr, size_t depth){
                locals.insert_or_assign(expr, depth);
            }

            void add_nesting_level(){
                env = make_shared<Environment>(env);
            }

            void remove_nesting_level(){
                if (env->get_enclosing() == nullptr){
                    return;
                }
                env = env->get_enclosing();
            }

            void run();
    };

    void run(const string& file_contents);

    namespace for_ast{
        VarValue look_up_var(const shared_ptr<Interpreter>& interpreter, const string& name, const ExprPtr& expr);

        void assign_var(const shared_ptr<Interpreter>& interpreter, const string& name, const ExprPtr& expr);

        shared_ptr<Environment> get_current_env(const shared_ptr<Interpreter>& interpreter);

        void add_nesting_level(const shared_ptr<Interpreter>& interpreter);

        void remove_nesting_level(const shared_ptr<Interpreter>& interpreter);

    }
}
