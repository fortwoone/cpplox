//
// Created by fortwoone on 06/04/2025.
//

#pragma once
#include "callable.hpp"
#include "instance.hpp"
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
    using std::enable_shared_from_this;
    using std::shared_ptr;

    class Expr;
}

namespace lox::interpreter{
    using lox::ast::Expr;
    using lox::callable::VarValue;
    using lox::env::Environment;
    using std::shared_ptr;
    using std::string;

    class Interpreter;

    namespace for_ast{
        VarValue look_up_var(const shared_ptr<Interpreter>& interpreter, const string& name, const shared_ptr<Expr>& expr);

        void assign_var(const shared_ptr<Interpreter>& interpreter, const string& name, const shared_ptr<Expr>& expr);

        shared_ptr<Environment> get_current_env(const shared_ptr<Interpreter>& interpreter);

        void add_nesting_level(const shared_ptr<Interpreter>& interpreter);

        void remove_nesting_level(const shared_ptr<Interpreter>& interpreter);
    }
}

namespace lox::ast {
    using lox::callable::CallablePtr;
    using lox::callable::EvalResult;
    using lox::callable::InstancePtr;
    using lox::callable::LoxClass;
    using lox::callable::LoxFunction;
    using lox::callable::MethodMap;
    using lox::env::Environment;
    using lox::inst::ClassPtr;
    using lox::inst::LoxInstance;
    using lox::interpreter::Interpreter;
    using lox::interpreter::for_ast::look_up_var;
    using lox::interpreter::for_ast::assign_var;
    using lox::interpreter::for_ast::get_current_env;
    using lox::interpreter::for_ast::add_nesting_level;
    using lox::interpreter::for_ast::remove_nesting_level;
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
#   define as_func get<CallablePtr>
#   define as_cls_inst get<InstancePtr>

    bool is_truthy(EvalResult eval_result);

    // region Expressions
    class Expr : public enable_shared_from_this<Expr> {
    public:
        virtual ~Expr() = default;

        [[nodiscard]] virtual string to_string() const = 0;

        [[nodiscard]] virtual EvalResult evaluate(const shared_ptr<Environment> &env) = 0;

        [[nodiscard]] virtual EvalResult evaluate(const shared_ptr<Interpreter> &interpreter) = 0;
    };


    enum class LiteralExprType : ubyte {
        TRUE,
        FALSE,
        NIL,
        STRING,
        NUMBER
    };

    LiteralExprType get_litexpr_tp_from_token_type(TokenType tp);


    // Literals
    class LiteralExpr : public Expr {
        string value;
    public:
        LiteralExprType expr_type;

        explicit LiteralExpr(LiteralExprType type) : expr_type(type) {}

        explicit LiteralExpr(LiteralExprType type, string value) : expr_type(type), value(std::move(value)) {}

        [[nodiscard]] string to_string() const final;

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Environment> &env) final;

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Interpreter> &interpreter) final;
    };

    enum class Operator : ubyte {
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

    class AbstractBinaryExpr : public Expr {
    protected:
        shared_ptr<Expr> left, right;
        Operator op;

    public:
        AbstractBinaryExpr(shared_ptr<Expr> left, Operator op, shared_ptr<Expr> right)
                : left(std::move(left)), op(op), right(std::move(right)) {}

        [[nodiscard]] shared_ptr<Expr> get_left() const {
            return left;
        }

        [[nodiscard]] shared_ptr<Expr> get_right() const {
            return right;
        }

        [[nodiscard]] string to_string() const final;

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Environment> &env) override = 0;

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Interpreter> &interpreter) override = 0;
    };

    class BinaryExpr : public AbstractBinaryExpr {
    public:
        BinaryExpr(shared_ptr<Expr> left, Operator op, shared_ptr<Expr> right)
                : AbstractBinaryExpr(std::move(left), op, std::move(right)) {}

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Environment> &env) final;

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Interpreter> &interpreter) final;
    };

    class GroupExpr : public Expr {
    public:
        shared_ptr<Expr> expr;

        explicit GroupExpr(shared_ptr<Expr> expression) : expr(std::move(expression)) {}

        [[nodiscard]] string to_string() const final {
            return "(group " + expr->to_string() + ")";
        }

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Environment> &env) final {
            return expr->evaluate(env);
        };

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Interpreter> &interpreter) final {
            return expr->evaluate(interpreter);
        }
    };

    class UnaryExpr : public Expr {
        Operator op;
        shared_ptr<Expr> operand;
    public:
        UnaryExpr(Operator operat, shared_ptr<Expr> expression) : op(operat), operand(std::move(expression)) {}

        [[nodiscard]] shared_ptr<Expr> get_operand() const {
            return operand;
        }

        [[nodiscard]] string to_string() const final;

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Environment> &env) final;

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Interpreter> &interpreter) final;
    };

    class AbstractVarExpr : public Expr {
    protected:
        string name;

    public:
        AbstractVarExpr(const string &name) : name(name) {};

        [[nodiscard]] string get_name() const {
            return name;
        };

        [[nodiscard]] string to_string() const final {
            return name;
        };

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Environment> &env) override = 0;

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Interpreter> &interpreter) override = 0;
    };

    class VariableExpr : public AbstractVarExpr {
    public:
        explicit VariableExpr(const Token &id_token);

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Environment> &env) final;

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Interpreter> &interpreter) final;
    };

    class AssignmentExpr : public AbstractVarExpr {
        shared_ptr<Expr> value;

    public:
        AssignmentExpr(string name, shared_ptr<Expr> value);

        [[nodiscard]] shared_ptr<Expr> get_value() const {
            return value;
        };

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Environment> &env) final;

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Interpreter> &interpreter) final;
    };

    class LogicalExpr : public AbstractBinaryExpr {
    public:
        LogicalExpr(shared_ptr<Expr> left, Operator op, shared_ptr<Expr> right)
                : AbstractBinaryExpr(std::move(left), op, std::move(right)) {
            if (op != Operator::AND && op != Operator::OR) {
                throw invalid_argument("Logical expressions cannot be constructed with non-logical operators.");
            }
        }

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Environment> &env) final;

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Interpreter> &interpreter) final;
    };

    class CallExpr : public Expr {
        shared_ptr<Expr> callee;
        Token &paren;
        vector<shared_ptr<Expr>> args;

    public:
        CallExpr(const shared_ptr<Expr> &callee, Token &paren, const vector<shared_ptr<Expr>> &args);

        [[nodiscard]] shared_ptr<Expr> get_callee() const {
            return callee;
        }

        [[nodiscard]] vector<shared_ptr<Expr>> get_args() const {
            return args;
        }

        [[nodiscard]] string to_string() const final;

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Environment> &env) final;

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Interpreter> &interpreter) final;
    };

    class AbstractInstAccessExpr : public Expr {
    protected:
        shared_ptr<Expr> obj;
        Token attr_token;
        string attr_name;

    public:
        explicit AbstractInstAccessExpr(const shared_ptr<Expr> &target, const Token &attr);

        [[nodiscard]] shared_ptr<Expr> get_obj() const {
            return obj;
        }

        [[nodiscard]] Token get_attr_token() const {
            return attr_token;
        }

        [[nodiscard]] string get_attr_name() const {
            return attr_name;
        }

        [[nodiscard]] string to_string() const override;

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Environment> &env) override = 0;

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Interpreter> &interpreter) override = 0;
    };

    class GetAttrExpr : public AbstractInstAccessExpr {
    public:
        explicit GetAttrExpr(const shared_ptr<Expr> &target, const Token &attr);

        [[nodiscard]] string to_string() const final;

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Environment> &env) final;

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Interpreter> &interpreter) final;
    };

    class SetAttrExpr : public AbstractInstAccessExpr {
        shared_ptr<Expr> value;

    public:
        explicit SetAttrExpr(const shared_ptr<Expr> &target, const Token &attr, const shared_ptr<Expr> &value);

        [[nodiscard]] shared_ptr<Expr> get_value() const {
            return value;
        }

        [[nodiscard]] string to_string() const final;

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Environment> &env) final;

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Interpreter> &interpreter) final;
    };

    class ThisExpr : public Expr {
        Token kw;

    public:
        explicit ThisExpr(const Token &token) : kw(token) {}

        [[nodiscard]] string to_string() const final {
            return "this";
        }

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Environment> &env) final;

        [[nodiscard]] EvalResult evaluate(const shared_ptr<Interpreter> &interpreter) final;
    };

    class SuperExpr;
}

namespace lox::interpreter::for_ast{
    size_t get_super_dist(const shared_ptr<Interpreter>& interpreter, const shared_ptr<ast::SuperExpr>& expr);
}

namespace lox::ast{
    using lox::interpreter::for_ast::get_super_dist;

    class SuperExpr: public Expr{
        Token kw, meth;

        public:
            explicit SuperExpr(const Token& kw, const Token& meth);

            [[nodiscard]] string to_string() const final{
                return "super";
            }

            [[nodiscard]] EvalResult evaluate(const shared_ptr<Environment>& env) final;
            [[nodiscard]] EvalResult evaluate(const shared_ptr<Interpreter>& interpreter) final;
    };
    // endregion

    // Base class for statements. A statement is an instruction executed by the interpreter.
    class Statement: public enable_shared_from_this<Statement>{
        public:
            virtual ~Statement() = default;
            virtual void execute(const shared_ptr<Environment>& env) = 0;
            virtual void execute(const shared_ptr<Interpreter>& interpreter) = 0;
    };

    class StatementWithExpr: public Statement{
        protected:
            shared_ptr<Expr> expr;
        public:
            explicit StatementWithExpr(shared_ptr<Expr> expr): expr(std::move(expr)){}

            [[nodiscard]] shared_ptr<Expr> get_expr() const{
                return expr;
            }

            void execute(const shared_ptr<Environment>& env) override = 0;
            void execute(const shared_ptr<Interpreter>& interpreter) override = 0;
    };

    class ExprStatement: public StatementWithExpr{
        public:
            explicit ExprStatement(shared_ptr<Expr> expr): StatementWithExpr(std::move(expr)){}

            void execute(const shared_ptr<Environment>& env) final;
            void execute(const shared_ptr<Interpreter>& interpreter) final;
    };

    class PrintStatement: public StatementWithExpr{
        public:
            explicit PrintStatement(shared_ptr<Expr> expr): StatementWithExpr(std::move(expr)){}

            void execute(const shared_ptr<Environment>& env) final;
            void execute(const shared_ptr<Interpreter>& interpreter) final;
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

            [[nodiscard]] bool has_initialiser() const{
                return (expr != nullptr);
            }

            void execute(const shared_ptr<Environment>& env) final;
            void execute(const shared_ptr<Interpreter>& interpreter) final;
    };

    class BlockStatement: public Statement{
        vector<shared_ptr<Statement>> statements;

        public:
            explicit BlockStatement(vector<shared_ptr<Statement>> statements);

            [[nodiscard]] vector<shared_ptr<Statement>> get_stmts() const{
                return statements;
            }

            void execute(const shared_ptr<Environment>& env) final;
            void execute(const shared_ptr<Interpreter>& interpreter) final;
    };

    class AbstractLogicalStmt: public Statement{
        protected:
            shared_ptr<Expr> condition;
            shared_ptr<Statement> on_success;

        public:
            AbstractLogicalStmt(shared_ptr<Expr> condition, shared_ptr<Statement> success);

            [[nodiscard]] shared_ptr<Expr> get_condition() const{
                return condition;
            }

            [[nodiscard]] shared_ptr<Statement> get_success() const{
                return on_success;
            }

            void execute(const shared_ptr<Environment>& env) override = 0;
            void execute(const shared_ptr<Interpreter>& interpreter) override = 0;
    };

    class IfStatement: public AbstractLogicalStmt{
        shared_ptr<Statement> on_failure;

        public:
            IfStatement(shared_ptr<Expr> condition, shared_ptr<Statement> success, shared_ptr<Statement> failure);
            IfStatement(shared_ptr<Expr> condition, shared_ptr<Statement> success);

            [[nodiscard]] bool has_failure() const{
                return (on_failure != nullptr);
            }

            [[nodiscard]] shared_ptr<Statement> get_failure() const{
                return on_failure;
            }

            void execute(const shared_ptr<Environment>& env) final;
            void execute(const shared_ptr<Interpreter>& interpreter) final;
    };

    class WhileStatement: public AbstractLogicalStmt{
        public:
            WhileStatement(shared_ptr<Expr> condition, shared_ptr<Statement> success);

            void execute(const shared_ptr<Environment>& env) final;
            void execute(const shared_ptr<Interpreter>& interpreter) final;
    };

    class FunctionStmt: public Statement{
        string name;
        vector<Token> args;
        vector<shared_ptr<Statement>> body;

        friend EvalResult for_callable::exec_func_body(const shared_ptr<Environment>& env, const shared_ptr<Statement>& func_stmt);
        friend EvalResult for_callable::exec_func_body(const shared_ptr<Interpreter>& interpreter, const shared_ptr<Statement>& func_stmt);
        friend vector<Token> for_callable::get_args(const shared_ptr<Statement>& func_stmt);
        friend string for_callable::get_func_name(const shared_ptr<Statement>& func_stmt);

        public:
            FunctionStmt(const Token& id_token, const vector<Token>& args, const vector<shared_ptr<Statement>>& body);

            [[nodiscard]] string get_name() const{
                return name;
            }

            [[nodiscard]] vector<Token> get_args() const{
                return args;
            }

            [[nodiscard]] vector<shared_ptr<Statement>> get_body() const{
                return body;
            }

            [[nodiscard]] ubyte get_arg_count() const{
                return static_cast<ubyte>(args.size());
            }

            void execute(const shared_ptr<Environment>& env) final;
            void execute(const shared_ptr<Interpreter>& interpreter) final;
    };

    class ReturnStmt: public StatementWithExpr{
        public:
            explicit ReturnStmt(const shared_ptr<Expr>& expr): StatementWithExpr(expr){}

            [[nodiscard]] bool has_val() const{
                return (expr != nullptr);
            }

            void execute(const shared_ptr<Environment>& env) final;
            void execute(const shared_ptr<Interpreter>& interpreter) final;
    };

    class ClassStmt: public Statement{
        string name;
        shared_ptr<VariableExpr> super_cls;
        vector<shared_ptr<FunctionStmt>> methods;

        public:
            explicit ClassStmt(const Token& id_token, const shared_ptr<VariableExpr>& superclass, const vector<shared_ptr<FunctionStmt>>& meths);

            [[nodiscard]] string get_name() const{
                return name;
            }

            [[nodiscard]] bool has_superclass() const{
                return super_cls != nullptr;
            }

            [[nodiscard]] shared_ptr<VariableExpr> get_superclass() const{
                return super_cls;
            }

            [[nodiscard]] vector<shared_ptr<FunctionStmt>> get_meths() const{
                return methods;
            }

            void execute(const shared_ptr<Environment>& env) final;
            void execute(const shared_ptr<Interpreter>& interpreter) final;
    };

    // Functions only used in callable context. Must not be used elsewhere.
    namespace for_callable{

        EvalResult exec_func_body(const shared_ptr<Interpreter>& interpreter, const shared_ptr<Statement>& func_stmt);

        string get_func_name(const shared_ptr<Statement>& func_stmt);

        vector<Token> get_args(const shared_ptr<Statement>& func_stmt);

        ubyte get_arg_count(const shared_ptr<Statement>& func_stmt);
    }
}

