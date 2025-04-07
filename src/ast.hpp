//
// Created by fortwoone on 06/04/2025.
//

#pragma once
#include "callable.hpp"
#include "env.hpp"
#include "exceptions.hpp"
#include "tokenizer.hpp"
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>

namespace lox::ast{
    using lox::callable::CallablePtr;
    using lox::callable::LoxFunction;
    using lox::callable::EvalResult;
    using lox::env::Environment;
    using lox::tokenizer::token::Token;
    using lox::tokenizer::token::TokenType;

    using std::boolalpha;
    using std::cout;
    using std::cerr;
    using std::dynamic_pointer_cast;
    using std::enable_shared_from_this;
    using std::endl;
    using std::fixed;
    using std::get;
    using std::invalid_argument;
    using std::make_shared;
    using std::noboolalpha;
    using std::ostringstream;
    using std::runtime_error;
    using std::setprecision;
    using std::shared_ptr;
    using std::string;
    using std::unordered_map;
    using std::unreachable;

#   define as_double get<double>
#   define as_bool get<bool>
#   define as_string get<string>

    bool is_truthy(EvalResult eval_result);

    // region Expressions
    class Expr{
        public:
            virtual ~Expr() = default;
            [[nodiscard]] virtual string to_string() const = 0;
            [[nodiscard]] virtual EvalResult evaluate() const = 0;
    };


    enum class LiteralExprType: ubyte{
        TRUE,
        FALSE,
        NIL,
        STRING,
        NUMBER
    };

    LiteralExprType get_litexpr_tp_from_token_type(TokenType tp);


    // Literals
    class LiteralExpr: public Expr{
        string value;
        public:
            LiteralExprType expr_type;

            explicit LiteralExpr(LiteralExprType type): expr_type(type){}
            explicit LiteralExpr(LiteralExprType type, string value): expr_type(type), value(std::move(value)){}

            [[nodiscard]] string to_string() const final;
            [[nodiscard]] EvalResult evaluate() const final;
    };

    enum class Operator: ubyte{
        PLUS,
        MINUS,
        STAR,
        SLASH,
        BANG,
        EQUALITY,
        INEQUALITY,
        LESS,
        GREATER,
        LESS_EQUAL,
        GREATER_EQUAL,
        AND,
        OR
    };

    Operator get_op_from_token(TokenType tp);

    class AbstractBinaryExpr: public Expr{
        protected:
            shared_ptr<Expr> left, right;
            Operator op;

        public:
            AbstractBinaryExpr(shared_ptr<Expr> left, Operator op, shared_ptr<Expr> right)
                    : left(std::move(left)), op(op), right(std::move(right)){}

            [[nodiscard]] string to_string() const final;

            [[nodiscard]] EvalResult evaluate() const override = 0;
    };

    class BinaryExpr: public AbstractBinaryExpr{
        public:
            BinaryExpr(shared_ptr<Expr> left, Operator op, shared_ptr<Expr> right)
                    : AbstractBinaryExpr(std::move(left), op, std::move(right)){}

            [[nodiscard]] EvalResult evaluate() const final;
    };

    class GroupExpr: public Expr{
        public:
            shared_ptr<Expr> expr;

            explicit GroupExpr(shared_ptr<Expr> expression): expr(std::move(expression)){}

            [[nodiscard]] string to_string() const final{
                return "(group " + expr->to_string() + ")";
            }

            [[nodiscard]] EvalResult evaluate() const final{
                return expr->evaluate();
            }
    };

    class UnaryExpr: public Expr{
        public:
            Operator op;
            shared_ptr<Expr> operand;

            UnaryExpr(Operator operat, shared_ptr<Expr> expression): op(operat), operand(std::move(expression)){}

            [[nodiscard]] string to_string() const final;

            [[nodiscard]] EvalResult evaluate() const final;
    };

    class AbstractVarExpr: public Expr{
        protected:
            string name;
            shared_ptr<Environment> env;

        public:
            AbstractVarExpr(const string& name, const shared_ptr<Environment>& env);

            [[nodiscard]] string get_name() const{
                return name;
            };

            [[nodiscard]] string to_string() const final{
                return name;
            };

            [[nodiscard]] EvalResult evaluate() const override = 0;

            void set_env(const shared_ptr<Environment>& env);
    };

    class VariableExpr: public AbstractVarExpr{
        public:
            explicit VariableExpr(const Token& id_token, const shared_ptr<Environment>& new_env);

            [[nodiscard]] EvalResult evaluate() const final;
    };

    class AssignmentExpr: public AbstractVarExpr{
        shared_ptr<Expr> value;

        public:
            AssignmentExpr(string name, shared_ptr<Expr> value, const shared_ptr<Environment>& env);

            [[nodiscard]] shared_ptr<Expr> get_value() const{
                return value;
            };

            [[nodiscard]] EvalResult evaluate() const final;
    };

    class LogicalExpr: public AbstractBinaryExpr{
        public:
            LogicalExpr(shared_ptr<Expr> left, Operator op, shared_ptr<Expr> right)
                    : AbstractBinaryExpr(std::move(left), op, std::move(right)){
                if (op != Operator::AND && op != Operator::OR){
                    throw invalid_argument("Logical expressions cannot be constructed with non-logical operators.");
                }
            }

            [[nodiscard]] EvalResult evaluate() const final;
    };

    class CallExpr: public Expr{
        shared_ptr<Environment> globals_env;
        shared_ptr<Expr> callee;
        Token& paren;
        vector<shared_ptr<Expr>> args;

        public:
            CallExpr(const shared_ptr<Expr>& callee, Token& paren, const vector<shared_ptr<Expr>>& args, const shared_ptr<Environment>& globals);

            [[nodiscard]] string to_string() const final;

            [[nodiscard]] EvalResult evaluate() const final;
    };
    // endregion

    // Base class for statements. A statement is an instruction executed by the interpreter.
    class Statement: public enable_shared_from_this<Statement>{
        public:
            virtual ~Statement() = default;
            virtual void execute() = 0;
    };

    class StatementWithExpr: public Statement{
        protected:
            shared_ptr<Expr> expr;
        public:
            explicit StatementWithExpr(shared_ptr<Expr> expr): expr(std::move(expr)){}

            [[nodiscard]] shared_ptr<Expr> get_expr() const{
                return expr;
            }

            void execute() override = 0;
    };

    class ExprStatement: public StatementWithExpr{
        shared_ptr<Environment> env;
        public:
            explicit ExprStatement(shared_ptr<Expr> expr): StatementWithExpr(std::move(expr)){}

            void execute() final;
    };

    class PrintStatement: public StatementWithExpr{
        public:
            explicit PrintStatement(shared_ptr<Expr> expr): StatementWithExpr(std::move(expr)){}

            void execute() final;
    };

    class VariableStatement: public StatementWithExpr{
        string name;

        public:
            explicit VariableStatement(const string& name, shared_ptr<Expr> init_expr);

            [[nodiscard]] string get_name() const{
                return name;
            }

            [[nodiscard]] shared_ptr<Expr> get_initialiser() const{
                return expr;
            }

            void execute() final;
    };

    class BlockStatement: public Statement{
        vector<shared_ptr<Statement>> statements;
        shared_ptr<Environment> env;

        void exec_var_stmt(const shared_ptr<VariableStatement>& var_stmt) const;

        public:
            explicit BlockStatement(vector<shared_ptr<Statement>> statements, const shared_ptr<Environment>& env);

            void execute() final;
    };

    class AbstractLogicalStmt: public Statement{
        protected:
            shared_ptr<Expr> condition;
            shared_ptr<Statement> on_success;

        public:
            AbstractLogicalStmt(shared_ptr<Expr> condition, shared_ptr<Statement> success);

            void execute() override = 0;
    };

    class IfStatement: public AbstractLogicalStmt{
        shared_ptr<Statement> on_failure;

        public:
            IfStatement(shared_ptr<Expr> condition, shared_ptr<Statement> success, shared_ptr<Statement> failure);
            IfStatement(shared_ptr<Expr> condition, shared_ptr<Statement> success);

            void execute() final;
    };

    class WhileStatement: public AbstractLogicalStmt{
        public:
            WhileStatement(shared_ptr<Expr> condition, shared_ptr<Statement> success);

            void execute() final;
    };

    class FunctionStmt: public Statement{
        string name;
        shared_ptr<Environment> globals_env;
        vector<Token> args;
        vector<shared_ptr<Statement>> body;

        friend vector<shared_ptr<Statement>> for_callable::get_func_body(const shared_ptr<Statement> &func_stmt);
        friend vector<Token> for_callable::get_args(const shared_ptr<Statement> &func_stmt);

        public:
            FunctionStmt(const Token& id_token, const vector<Token>& args, const vector<shared_ptr<Statement>>& body, const shared_ptr<Environment>& globals);

            [[nodiscard]] ubyte get_arg_count() const{
                return static_cast<ubyte>(args.size());
            }

            void execute() final;
    };

    // Functions only used in callable context. Must not be used elsewhere.
    namespace for_callable{
        vector<shared_ptr<Statement>> get_func_body(const shared_ptr<Statement> &func_stmt);

        vector<Token> get_args(const shared_ptr<Statement> &func_stmt);

        ubyte get_arg_count(const shared_ptr<Statement> &func_stmt);

        void exec_stmt_list_as_block(const vector<shared_ptr<Statement>> &stmts, const shared_ptr<Environment> &env);
    }
}

