//
// Created by fortwoone on 08/04/2025.
//

#include "resolver.hpp"

namespace lox::resolver{
    void Resolver::start_scope(){
        scope_stack.emplace_back();
    }

    void Resolver::finish_scope(){
        scope_stack.pop_back();
    }

    void Resolver::declare(const string& name){
        if (scope_stack.empty()){
            return;
        }
        if (scope_stack.back().contains(name)){
            throw resolve_error("Current scope already has a variable with this name.");
        }

        scope_stack.back().insert({name, false});
    }

    void Resolver::define(const string& name){
        if (scope_stack.empty()){
            return;
        }

        scope_stack.back().insert_or_assign(name, true);
    }

    void Resolver::resolve_local(const shared_ptr<ast::Expr>& expr, const string& name){
        for (ssize_t i = scope_stack.size() - 1; i >= 0; --i){
            if (scope_stack.at(i).contains(name)){
                return interpreter->resolve(expr, scope_stack.size() - 1 - i);
            }
        }
    }

    void Resolver::resolve_func(const shared_ptr<ast::FunctionStmt>& stmt, FuncType tp){
        FuncType enclosing = current_func;
        current_func = tp;
        start_scope();
        for (const auto& arg: stmt->get_args()){
            declare(arg.get_lexeme());
            define(arg.get_lexeme());
        }
        resolve(stmt->get_body());
        finish_scope();
        current_func = enclosing;
    }

    // region Resolve method for individual expression types
    void Resolver::resolve_abstract_bin_expr(const shared_ptr<ast::AbstractBinaryExpr>& bin_expr){
        resolve(bin_expr->get_left());
        resolve(bin_expr->get_right());
    }

    void Resolver::resolve_var_expr(const shared_ptr<ast::VariableExpr>& var_expr){
        if (!scope_stack.empty()){
            if (scope_stack.back().contains(var_expr->get_name()) && !scope_stack.back().at(var_expr->get_name()))
                throw resolve_error("Can't read local variable in its own initialiser.\0");
        }

        resolve_local(var_expr, var_expr->get_name());
    }

    void Resolver::resolve_assign_expr(const shared_ptr<ast::AssignmentExpr>& assign_expr){
        resolve(assign_expr->get_value());
        resolve_local(assign_expr->get_value(), assign_expr->get_name());
    }

    void Resolver::resolve_unary_expr(const shared_ptr<ast::UnaryExpr>& unary_expr){
        resolve(unary_expr->get_operand());
    }

    void Resolver::resolve_call_expr(const shared_ptr<ast::CallExpr>& call_expr){
        resolve(call_expr->get_callee());

        for (const auto& arg: call_expr->get_args()){
            resolve(arg);
        }
    }

    void Resolver::resolve_get_expr(const shared_ptr<ast::GetAttrExpr>& get_attr_expr){
        resolve(get_attr_expr->get_obj());
    }

    void Resolver::resolve_set_expr(const shared_ptr<ast::SetAttrExpr>& set_attr_expr){
        resolve(set_attr_expr->get_value());
        resolve(set_attr_expr->get_obj());
    }

    void Resolver::resolve_this_expr(const shared_ptr<ast::ThisExpr>& this_expr){
        if (current_cls == ClassType::NONE){
            throw resolve_error("Can't use 'this' outside of classes.");
        }
        resolve_local(this_expr, "this");
    }
    // endregion

    // region Resolve methods for individual statement types
    void Resolver::resolve_class_stmt(const shared_ptr<ast::ClassStmt>& class_stmt){
        ClassType enclosing_cls = current_cls;
        current_cls = ClassType::CLASS;

        declare(class_stmt->get_name());
        define(class_stmt->get_name());

        start_scope();
        scope_stack.back().insert({"this", true});  // Allow the resolver to take care of this expressions.

        FuncType decl = FuncType::METHOD;
        for (const auto& meth: class_stmt->get_meths()){
            if (meth->get_name() == "init"){
                decl = FuncType::INITIALISER;
            }

            resolve_func(meth, decl);
            decl = FuncType::METHOD;
        }

        finish_scope();
        current_cls = enclosing_cls;
    }

    void Resolver::resolve_block_stmt(const shared_ptr<ast::BlockStatement>& block_stmt){
        start_scope();
        resolve(block_stmt->get_stmts());
        finish_scope();
    }

    void Resolver::resolve_variable_stmt(const shared_ptr<ast::VariableStatement>& var_stmt){
        declare(var_stmt->get_name());
        if (var_stmt->has_initialiser()){
            resolve(var_stmt->get_initialiser());
        }
        define(var_stmt->get_name());
    }

    void Resolver::resolve_func_stmt(const shared_ptr<ast::FunctionStmt>& func_stmt){
        declare(func_stmt->get_name());
        define(func_stmt->get_name());

        resolve_func(func_stmt, FuncType::FUNCTION);
    }

    void Resolver::resolve_if_stmt(const shared_ptr<ast::IfStatement>& if_stmt){
        resolve(if_stmt->get_condition());
        resolve(if_stmt->get_success());
        if (if_stmt->has_failure()){
            resolve(if_stmt->get_failure());
        }
    }

    void Resolver::resolve_while_stmt(const shared_ptr<ast::WhileStatement>& while_stmt){
        resolve(while_stmt->get_condition());
        resolve(while_stmt->get_success());
    }

    void Resolver::resolve_ret_stmt(const shared_ptr<ast::ReturnStmt>& ret_stmt){
        if (current_func == FuncType::NONE){
            throw resolve_error("Cannot return from top-level code.");
        }
        if (current_func == FuncType::INITIALISER && ret_stmt->has_val()){
            throw resolve_error("Cannot return a value from an initialiser.");
        }
        if (ret_stmt->has_val())
            resolve(ret_stmt->get_expr());
    }

    void Resolver::resolve_swe_stmt(const shared_ptr<ast::StatementWithExpr>& swe_ptr){
        resolve(swe_ptr->get_expr());
    }
    // endregion

    void Resolver::resolve(const shared_ptr<ast::Expr>& expr){
        // Literals do not need to be resolved.
        auto as_assign = dynamic_pointer_cast<ast::AssignmentExpr>(expr);
        if (as_assign != nullptr){
            return resolve_assign_expr(as_assign);
        }
        auto as_var_expr = dynamic_pointer_cast<ast::VariableExpr>(expr);
        if (as_var_expr != nullptr){
            return resolve_var_expr(as_var_expr);
        }
        auto as_bin_expr = dynamic_pointer_cast<ast::AbstractBinaryExpr>(expr);
        if (as_bin_expr != nullptr){
            return resolve_abstract_bin_expr(as_bin_expr);
        }
        auto as_un_expr = dynamic_pointer_cast<ast::UnaryExpr>(expr);
        if (as_un_expr != nullptr){
            return resolve_unary_expr(as_un_expr);
        }
        auto as_call_expr = dynamic_pointer_cast<ast::CallExpr>(expr);
        if (as_call_expr != nullptr){
            return resolve_call_expr(as_call_expr);
        }
        auto as_this = dynamic_pointer_cast<ast::ThisExpr>(expr);
        if (as_this != nullptr){
            return resolve_this_expr(as_this);
        }
        auto as_set_attr = dynamic_pointer_cast<ast::SetAttrExpr>(expr);
        if (as_set_attr != nullptr){
            return resolve_set_expr(as_set_attr);
        }
        auto as_get_attr = dynamic_pointer_cast<ast::GetAttrExpr>(expr);
        if (as_get_attr != nullptr){
            return resolve_get_expr(as_get_attr);
        }
    }

    void Resolver::resolve(const shared_ptr<ast::Statement>& stmt){
        auto as_class_stmt = dynamic_pointer_cast<ast::ClassStmt>(stmt);
        if (as_class_stmt != nullptr){
            return resolve_class_stmt(as_class_stmt);
        }
        auto as_block_stmt = dynamic_pointer_cast<ast::BlockStatement>(stmt);
        if (as_block_stmt != nullptr){
            return resolve_block_stmt(as_block_stmt);
        }
        auto as_var_stmt = dynamic_pointer_cast<ast::VariableStatement>(stmt);
        if (as_var_stmt != nullptr){
            return resolve_variable_stmt(as_var_stmt);
        }
        auto as_func_stmt = dynamic_pointer_cast<ast::FunctionStmt>(stmt);
        if (as_func_stmt != nullptr){
            return resolve_func_stmt(as_func_stmt);
        }
        auto as_if_stmt = dynamic_pointer_cast<ast::IfStatement>(stmt);
        if (as_if_stmt != nullptr){
            return resolve_if_stmt(as_if_stmt);
        }
        auto as_while_stmt = dynamic_pointer_cast<ast::WhileStatement>(stmt);
        if (as_while_stmt != nullptr){
            return resolve_while_stmt(as_while_stmt);
        }
        auto as_ret_stmt = dynamic_pointer_cast<ast::ReturnStmt>(stmt);
        if (as_ret_stmt != nullptr){
            return resolve_ret_stmt(as_ret_stmt);
        }
        auto as_swe_ptr = dynamic_pointer_cast<ast::StatementWithExpr>(stmt);
        if (as_swe_ptr != nullptr){
            return resolve_swe_stmt(as_swe_ptr);
        }
    }

    void Resolver::resolve(const vector<shared_ptr<ast::Statement>>& statements){
        for (const auto& stmt: statements){
            resolve(stmt);
        }
    }
}
