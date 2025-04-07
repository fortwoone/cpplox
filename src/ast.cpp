//
// Created by fortwoone on 06/04/2025.
//

#include "ast.hpp"

namespace lox::ast{
    bool is_truthy(EvalResult eval_result){  // Truth operator check.
        if (holds_alternative<string>(eval_result)){
            return as_string(eval_result) != "nil";
        }
        else if (holds_alternative<bool>(eval_result)){
            return as_bool(eval_result);
        }
        return true;
    }

    // region AST constants
    const unordered_map<TokenType, LiteralExprType> _TOKEN_TO_LITEXPRTP{
            {TokenType::TRUE, LiteralExprType::TRUE},
            {TokenType::FALSE, LiteralExprType::FALSE},
            {TokenType::NIL, LiteralExprType::NIL},
            {TokenType::STRING, LiteralExprType::STRING},
            {TokenType::NUMBER, LiteralExprType::NUMBER},
    };

    LiteralExprType get_litexpr_tp_from_token_type(TokenType tp){
        return _TOKEN_TO_LITEXPRTP.at(tp);
    }

    const unordered_map<TokenType, Operator> _TOKEN_TO_OP{
            {TokenType::PLUS, Operator::PLUS},
            {TokenType::MINUS, Operator::MINUS},
            {TokenType::STAR, Operator::STAR},
            {TokenType::SLASH, Operator::SLASH},
            {TokenType::BANG, Operator::BANG},
            {TokenType::EQUAL_EQUAL, Operator::EQUALITY},
            {TokenType::BANG_EQUAL, Operator::INEQUALITY},
            {TokenType::LESS, Operator::LESS},
            {TokenType::GREATER, Operator::GREATER},
            {TokenType::LESS_EQUAL, Operator::LESS_EQUAL},
            {TokenType::GREATER_EQUAL, Operator::GREATER_EQUAL},
            {TokenType::AND, Operator::AND},
            {TokenType::OR, Operator::OR}
    };

    Operator get_op_from_token(TokenType tp){
        return _TOKEN_TO_OP.at(tp);
    }

    const unordered_map<Operator, string> _OP_TO_SYM{
            {Operator::PLUS, "+"},
            {Operator::MINUS, "-"},
            {Operator::STAR, "*"},
            {Operator::SLASH, "/"},
            {Operator::BANG, "!"},
            {Operator::EQUALITY, "=="},
            {Operator::INEQUALITY, "!="},
            {Operator::LESS, "<"},
            {Operator::GREATER, ">"},
            {Operator::LESS_EQUAL, "<="},
            {Operator::GREATER_EQUAL, ">="},
            {Operator::AND, "and"},
            {Operator::OR, "or"}
    };
    // endregion

    // region LiteralExpr
    string LiteralExpr::to_string() const{
        switch (expr_type){
            case LiteralExprType::TRUE:
                return "true";
            case LiteralExprType::FALSE:
                return "false";
            case LiteralExprType::NIL:
                return "nil";
            default:
                return value;
        }
    }

    EvalResult LiteralExpr::evaluate() const{
        switch (expr_type){
            case LiteralExprType::TRUE:
                return true;
            case LiteralExprType::FALSE:
                return false;
            case LiteralExprType::NIL:
                return "nil";
            case LiteralExprType::NUMBER:
                return stod(value);
            default:
                return value;
        }
    }
    // endregion

    // region AbstractBinaryExpr
    string AbstractBinaryExpr::to_string() const{
        return "(" + _OP_TO_SYM.at(op) + " " + left->to_string() + " " + right->to_string() + ")";
    }
    // endregion

    // region BinaryExpr
    EvalResult BinaryExpr::evaluate() const{
        using enum Operator;
        EvalResult left_result = left->evaluate(), right_result = right->evaluate();
        bool two_numbers = holds_alternative<double>(left_result) && holds_alternative<double>(right_result);
        bool two_bools = holds_alternative<bool>(left_result) && holds_alternative<bool>(right_result);
        bool two_strings = holds_alternative<string>(left_result) && holds_alternative<string>(right_result);

        switch (op){
            case PLUS:
                if (two_numbers){
                    return as_double(left_result) + as_double(right_result);
                }
                else if (two_strings){
                    ostringstream concat_stream;
                    concat_stream << as_string(left_result) << as_string(right_result);
                    return concat_stream.str();
                }
                break;
            case MINUS:
                if (two_numbers){
                    return as_double(left_result) - as_double(right_result);
                }
                break;
            case STAR:
                if (two_numbers){
                    return as_double(left_result) * as_double(right_result);
                }
                break;
            case SLASH:
                if (two_numbers){
                    return as_double(left_result) / as_double(right_result);
                }
                break;
            case LESS:
                if (two_numbers){
                    return as_double(left_result) < as_double(right_result);
                }
                else{
                    throw parse_error(70, "Unsupported operation.\0");
                }
            case GREATER:
                if (two_numbers){
                    return as_double(left_result) > as_double(right_result);
                }
                else{
                    throw parse_error(70, "Unsupported operation.\0");
                }
            case LESS_EQUAL:
                if (two_numbers){
                    return as_double(left_result) <= as_double(right_result);
                }
                else{
                    throw parse_error(70, "Unsupported operation.\0");
                }
            case GREATER_EQUAL:
                if (two_numbers){
                    return as_double(left_result) >= as_double(right_result);
                }
                else {
                    throw parse_error(70, "Unsupported operation.\0");
                }
            case EQUALITY:
                if (two_numbers){
                    return as_double(left_result) == as_double(right_result);
                }
                else if (two_bools){
                    return as_bool(left_result) == as_bool(right_result);
                }
                else if (two_strings){
                    return as_string(left_result) == as_string(right_result);
                }
                return false;
            case INEQUALITY:
                if (two_numbers){
                    return as_double(left_result) != as_double(right_result);
                }
                else if (two_bools){
                    return as_bool(left_result) != as_bool(right_result);
                }
                else if (two_strings){
                    return as_string(left_result) != as_string(right_result);
                }
                return true;
            default:
                throw parse_error(70, "Unsupported operation.\0");
        }
        throw parse_error(70, "Unsupported operation.\0");
    }
    // endregion

    // region UnaryExpr
    string UnaryExpr::to_string() const{
        return "(" + _OP_TO_SYM.at(op) + " " + operand->to_string() + ")";
    }

    EvalResult UnaryExpr::evaluate() const{
        EvalResult evaluated_operand = operand->evaluate();

        if (holds_alternative<bool>(evaluated_operand)){
            if (op == Operator::BANG){
                return !as_bool(evaluated_operand);
            }
        }
        else if (holds_alternative<double>(evaluated_operand)){
            switch (op){
                case Operator::MINUS:
                    return -as_double(evaluated_operand);
                case Operator::BANG:
                    return false;
                default:
                    break;
            }
        }
        else{
            switch (op){
                case Operator::BANG:
                    return as_string(evaluated_operand) == "nil";
                default:
                    break;
            }
        }
        throw parse_error(70, "Invalid operand for unary expression.\0");
    }
    // endregion

    // region AbstractVarExpr
    AbstractVarExpr::AbstractVarExpr(const string& name, const shared_ptr<Environment>& env)
            : name(name), env(env){
    }

    void AbstractVarExpr::set_env(const shared_ptr<Environment>& new_env){
        this->env = new_env;
    }
    // endregion

    // region VariableExpr
    // Using const references to shared pointers to make absolutely SURE memory doesn't get out of hand when
    // handing a pointer to the environment object.
    VariableExpr::VariableExpr(const Token& id_token, const shared_ptr<Environment>& env): AbstractVarExpr(id_token.get_lexeme(), env){
        if (id_token.get_token_type() != TokenType::IDENTIFIER){
            throw parse_error(65, "Invalid token type for variable expression.");
        }
    }

    EvalResult VariableExpr::evaluate() const{
        if (env == nullptr){
            return name;
        }
        return env->get(name);
    }
    // endregion

    // region AssignmentExpr
    AssignmentExpr::AssignmentExpr(string name, shared_ptr<Expr> value, const shared_ptr<Environment>& env)
            : AbstractVarExpr(std::move(name), env), value(std::move(value)){}

    EvalResult AssignmentExpr::evaluate() const{
        EvalResult val = value->evaluate();
        env->assign(name, val);
        return val;
    }
    // endregion

    // region LogicalExpr
    EvalResult LogicalExpr::evaluate() const{
        EvalResult left_evaled = left->evaluate();

        switch (op){
            case Operator::OR:
                return (is_truthy(left_evaled) ? left_evaled : right->evaluate());
            case Operator::AND:
                return (is_truthy(left_evaled) ? right->evaluate() : left_evaled);
            default:
                unreachable();
        }
    }
    // endregion

    // region CallExpr
    CallExpr::CallExpr(const shared_ptr<Expr>& callee, Token& paren, const vector<shared_ptr<Expr>>& args, const shared_ptr<Environment>& globals)
            : callee(callee), paren(paren), args(args), globals_env(globals){}

    string CallExpr::to_string() const{
        auto ret = callee->to_string() + "(";

        for (const auto& arg: args){
            ret += arg->to_string();
            if (arg != args.back()){
                ret += ", ";
            }
        }
        ret += ")";

        return ret;
    };

    EvalResult CallExpr::evaluate() const{
        EvalResult callee_eval = callee->evaluate();

        if (!is_callable(callee_eval)){
            throw runtime_error("Given object is not callable.");
        }

        vector<EvalResult> args_evaled;
        args_evaled.reserve(args.size());
        for (const auto& arg: args){
            args_evaled.push_back(arg->evaluate());
        }


        CallablePtr func = as_func(callee_eval);

        if (args_evaled.size() != func->arity()){
            throw runtime_error("Expected " + std::to_string(func->arity()) + " arguments, got " + std::to_string(args_evaled.size()));
        }

        return func->call(globals_env, args_evaled);
    }
    // endregion

    // region ExprStatement
    void ExprStatement::execute(){
        EvalResult result = expr->evaluate();
    }
    // endregion

    // region PrintStatement
    void PrintStatement::execute(){
        EvalResult result = expr->evaluate();
        if (holds_alternative<bool>(result)){
            cout << (as_bool(result) ? "true" : "false") << endl;
        }
        else if (holds_alternative<double>(result)){
            auto previous = cout.precision();
            if (static_cast<int>(as_double(result)) == as_double(result)){
                cout << fixed << setprecision(0);
            }
            cout << as_double(result) << endl;
            if (static_cast<int>(as_double(result)) == as_double(result)){
                cout << setprecision(previous);
                cout.unsetf(std::ios_base::fixed);
            }
        }
        else if (holds_alternative<CallablePtr>(result)){
            cout << as_func(result)->to_string() << endl;
        }
        else{
            cout << as_string(result) << endl;
        }
    }
    // endregion

    // region VariableStatement
    VariableStatement::VariableStatement(const string& name, shared_ptr<Expr> init_expr): name(name), StatementWithExpr(init_expr){}  // NOLINT

    void VariableStatement::execute(){
        // Does nothing. Only exists for RTTI purposes. The interpreter will be the actual one doing the variable setting.
    }
    // endregion

    // region BlockStatement
    BlockStatement::BlockStatement(vector<shared_ptr<Statement>> statements, const shared_ptr<Environment>& env)
            : statements(std::move(statements)), env(env){}

    void BlockStatement::exec_var_stmt(const shared_ptr<VariableStatement>& var_stmt) const{
        EvalResult val = "nil";
        if (var_stmt->get_initialiser() != nullptr){
            val = var_stmt->get_initialiser()->evaluate();
        }

        env->set(var_stmt->get_name(), val);
    }

    void BlockStatement::execute(){
        for (const auto& stmt: statements){
            auto as_swe_ptr = dynamic_pointer_cast<StatementWithExpr>(stmt);
            if (as_swe_ptr != nullptr){
                auto as_var_expr = dynamic_pointer_cast<AbstractVarExpr>(as_swe_ptr->get_expr());
                if (as_var_expr != nullptr){
                    // Set variable expressions' environment as the block one instead of the top-level one.
                    as_var_expr->set_env(env);
                }
            }
            auto as_var_stmt = dynamic_pointer_cast<VariableStatement>(stmt);
            if (as_var_stmt != nullptr){
                exec_var_stmt(as_var_stmt);
                continue;
            }
            stmt->execute();
        }
    }

    namespace for_callable{
        void exec_stmt_list_as_block(const vector<shared_ptr<Statement>>& stmts, const shared_ptr<Environment>& env) {
            shared_ptr<BlockStatement> as_block = make_shared<BlockStatement>(stmts, env);
            as_block->execute();
        }
    }
    // endregion

    // region AbstractLogicalStmt
    AbstractLogicalStmt::AbstractLogicalStmt(shared_ptr<Expr> condition, shared_ptr<Statement> success)
            : condition(std::move(condition)), on_success(std::move(success)){}
    // endregion

    // region IfStatement
    IfStatement::IfStatement(shared_ptr<Expr> condition, shared_ptr<Statement> success, shared_ptr<Statement> failure)
            : AbstractLogicalStmt(std::move(condition), std::move(success)), on_failure(std::move(failure)){}

    IfStatement::IfStatement(shared_ptr<Expr> condition, shared_ptr<Statement> success)
            : AbstractLogicalStmt(std::move(condition), std::move(success)), on_failure(nullptr){}

    void IfStatement::execute(){
        EvalResult condit_evaled = condition->evaluate();
        if (is_truthy(condit_evaled)){
            return on_success->execute();
        }
        if (on_failure != nullptr){
            return on_failure->execute();
        }
    }
    // endregion

    // region WhileStatement
    WhileStatement::WhileStatement(shared_ptr<Expr> condition, shared_ptr<Statement> success)
            : AbstractLogicalStmt(std::move(condition), std::move(success)){}

    void WhileStatement::execute(){
        while (is_truthy(condition->evaluate())){
            on_success->execute();
        }
    }
    // endregion

    // region FunctionStmt
    FunctionStmt::FunctionStmt(const Token& id_token, const vector<Token>& args, const vector<shared_ptr<Statement>>& body, const shared_ptr<Environment>& globals)
    : args(args), body(body), globals_env(globals){
        name = id_token.get_lexeme();
        if (args.size() >= 255){
            throw invalid_argument("Cannot have 255 parameters or more in a function.");
        }
    }

    namespace for_callable{
        vector<shared_ptr<Statement>> get_func_body(const shared_ptr<Statement> &func_stmt) {
            vector<shared_ptr<Statement>> ret;
            auto as_func_stmt = dynamic_pointer_cast<FunctionStmt>(func_stmt);
            if (as_func_stmt == nullptr) {
                return ret;
            }

            return {as_func_stmt->body};
        }

        string get_func_name(const shared_ptr<Statement>& func_stmt){
            auto as_func_stmt = dynamic_pointer_cast<FunctionStmt>(func_stmt);
            if (as_func_stmt != nullptr) {
                return as_func_stmt->name;
            }

            return "";
        }

        vector<Token> get_args(const shared_ptr<Statement>& func_stmt) {
            vector<Token> ret;
            auto as_func_stmt = dynamic_pointer_cast<FunctionStmt>(func_stmt);
            if (as_func_stmt == nullptr) {
                return ret;
            }

            return {as_func_stmt->args};
        }

        ubyte get_arg_count(const shared_ptr<Statement>& func_stmt) {
            auto as_func_stmt = dynamic_pointer_cast<FunctionStmt>(func_stmt);
            if (as_func_stmt == nullptr) {
                return 0;
            }
            return as_func_stmt->get_arg_count();
        }
    }

    void FunctionStmt::execute(){
        shared_ptr<Statement> shared = shared_from_this();
        globals_env->set(
            name,
            make_shared<LoxFunction>(shared)
        );
    }
    // endregion
}
