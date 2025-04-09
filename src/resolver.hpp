//
// Created by fortwoone on 08/04/2025.
//

#pragma once
#include "ast.hpp"
#include "exceptions.hpp"
#include "interpreter.hpp"
#include <cstdint>
#include <deque>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Actual declarations
namespace lox::resolver{
    using lox::interpreter::Interpreter;
    using lox::tokenizer::token::Token;

    using std::deque;
    using std::dynamic_pointer_cast;
    using std::enable_shared_from_this;
    using std::make_shared;
    using std::shared_ptr;
    using std::string;
    using std::unordered_map;
    using std::vector;

    using Scope = unordered_map<string, bool>;

    enum class FuncType: ubyte{
        NONE,
        FUNCTION,
        INITIALISER,
        METHOD
    };

    enum class ClassType: ubyte{
        NONE,
        CLASS,
        SUBCLASS
    };

    class Resolver: public enable_shared_from_this<Resolver>{
        shared_ptr<Interpreter> interpreter;
        deque<Scope> scope_stack;
        FuncType current_func;
        ClassType current_cls;

        void start_scope();
        void finish_scope();

        void declare(const string& name);
        void define(const string& name);

        void resolve_local(const shared_ptr<ast::Expr>& expr, const string& name);
        void resolve_func(const shared_ptr<ast::FunctionStmt>& stmt, FuncType tp);

        // region Resolve methods for individual expression types
        void resolve_abstract_bin_expr(const shared_ptr<ast::AbstractBinaryExpr>& bin_expr);
        void resolve_var_expr(const shared_ptr<ast::VariableExpr>& var_expr);
        void resolve_assign_expr(const shared_ptr<ast::AssignmentExpr>& assign_expr);
        void resolve_unary_expr(const shared_ptr<ast::UnaryExpr>& unary_expr);
        void resolve_call_expr(const shared_ptr<ast::CallExpr>& call_expr);
        void resolve_get_expr(const shared_ptr<ast::GetAttrExpr>& get_attr_expr);
        void resolve_set_expr(const shared_ptr<ast::SetAttrExpr>& set_attr_expr);
        void resolve_this_expr(const shared_ptr<ast::ThisExpr>& this_expr);
        void resolve_super_expr(const shared_ptr<ast::SuperExpr>& super_expr);
        // endregion

        // region Resolve methods for individual statement types
        void resolve_class_stmt(const shared_ptr<ast::ClassStmt>& class_stmt);
        void resolve_block_stmt(const shared_ptr<ast::BlockStatement>& block_stmt);
        void resolve_variable_stmt(const shared_ptr<ast::VariableStatement>& var_stmt);
        void resolve_func_stmt(const shared_ptr<ast::FunctionStmt>& func_stmt);
        void resolve_if_stmt(const shared_ptr<ast::IfStatement>& if_stmt);
        void resolve_while_stmt(const shared_ptr<ast::WhileStatement>& while_stmt);
        void resolve_ret_stmt(const shared_ptr<ast::ReturnStmt>& ret_stmt);
        void resolve_swe_stmt(const shared_ptr<ast::StatementWithExpr>& swe_ptr);
        // endregion

        void resolve(const shared_ptr<ast::Expr>& expr);
        void resolve(const shared_ptr<ast::Statement>& stmt);

        public:
            explicit Resolver(const shared_ptr<Interpreter>& interpreter): interpreter(interpreter){
                current_func = FuncType::NONE;
                current_cls = ClassType::NONE;
            }

            void resolve(const vector<shared_ptr<ast::Statement>>& statements);


    };
}
