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

    EvalResult LiteralExpr::evaluate(const shared_ptr<Environment>& env){
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

    EvalResult LiteralExpr::evaluate(const shared_ptr<Interpreter>& interpreter){
        return evaluate(get_current_env(interpreter));
    }
    // endregion

    // region AbstractBinaryExpr
    string AbstractBinaryExpr::to_string() const{
        return "(" + _OP_TO_SYM.at(op) + " " + left->to_string() + " " + right->to_string() + ")";
    }
    // endregion

    // region BinaryExpr
    EvalResult BinaryExpr::evaluate(const shared_ptr<Environment>& env){
        using enum Operator;
        EvalResult left_result = left->evaluate(env), right_result = right->evaluate(env);
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

    EvalResult BinaryExpr::evaluate(const shared_ptr<Interpreter>& interpreter){
        using enum Operator;
        EvalResult left_result = left->evaluate(interpreter), right_result = right->evaluate(interpreter);
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

    EvalResult UnaryExpr::evaluate(const shared_ptr<Environment>& env){
        EvalResult evaluated_operand = operand->evaluate(env);

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

    EvalResult UnaryExpr::evaluate(const shared_ptr<Interpreter>& interpreter){
        EvalResult evaluated_operand = operand->evaluate(interpreter);

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

    // region VariableExpr
    // Using const references to shared pointers to make absolutely SURE memory doesn't get out of hand when
    // handing a pointer to the environment object.
    VariableExpr::VariableExpr(const Token& id_token): AbstractVarExpr(id_token.get_lexeme()){
        if (id_token.get_token_type() != TokenType::IDENTIFIER){
            throw parse_error(65, "Invalid token type for variable expression.");
        }
    }

    EvalResult VariableExpr::evaluate(const shared_ptr<Environment>& env){
        return env->get(name);
    }

    EvalResult VariableExpr::evaluate(const shared_ptr<Interpreter>& interpreter){
        return look_up_var(interpreter, name, shared_from_this());
    }
    // endregion

    // region AssignmentExpr
    AssignmentExpr::AssignmentExpr(string name, shared_ptr<Expr> value)
            : AbstractVarExpr(std::move(name)), value(std::move(value)){}

    EvalResult AssignmentExpr::evaluate(const shared_ptr<Environment>& env){
        EvalResult val = value->evaluate(env);
        env->assign(name, val);
        return val;
    }

    EvalResult AssignmentExpr::evaluate(const shared_ptr<Interpreter>& interpreter){
        EvalResult evaled = value->evaluate(interpreter);
        assign_var(interpreter, name, value);
        return evaled;
    }
    // endregion

    // region LogicalExpr
    EvalResult LogicalExpr::evaluate(const shared_ptr<Environment>& env){
        EvalResult left_evaled = left->evaluate(env);

        switch (op){
            case Operator::OR:
                return (is_truthy(left_evaled) ? left_evaled : right->evaluate(env));
            case Operator::AND:
                return (is_truthy(left_evaled) ? right->evaluate(env) : left_evaled);
            default:
                unreachable();
        }
    }

    EvalResult LogicalExpr::evaluate(const shared_ptr<Interpreter>& interpreter){
        EvalResult left_evaled = left->evaluate(interpreter);

        switch (op){
            case Operator::OR:
                return (is_truthy(left_evaled) ? left_evaled : right->evaluate(interpreter));
            case Operator::AND:
                return (is_truthy(left_evaled) ? right->evaluate(interpreter) : left_evaled);
            default:
                unreachable();
        }
    }
    // endregion

    // region CallExpr
    CallExpr::CallExpr(const shared_ptr<Expr>& callee, Token& paren, const vector<shared_ptr<Expr>>& args)
            : callee(callee), paren(paren), args(args){}

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

    EvalResult CallExpr::evaluate(const shared_ptr<Environment>& env){
        EvalResult callee_eval = callee->evaluate(env);

        if (!is_callable(callee_eval)){
            throw runtime_error("Given object is not callable.");
        }

        vector<EvalResult> args_evaled;
        args_evaled.reserve(args.size());
        for (const auto& arg: args){
            args_evaled.push_back(arg->evaluate(env));
        }


        CallablePtr func = as_func(callee_eval);

        if (args_evaled.size() != func->arity()){
            throw runtime_error("Expected " + std::to_string(func->arity()) + " arguments, got " + std::to_string(args_evaled.size()));
        }

        return func->call(env, args_evaled);
    }

    EvalResult CallExpr::evaluate(const shared_ptr<Interpreter>& interpreter){
        EvalResult callee_eval = callee->evaluate(interpreter);

        if (!is_callable(callee_eval)){
            throw runtime_error("Given object is not callable.");
        }

        vector<EvalResult> args_evaled;
        args_evaled.reserve(args.size());
        for (const auto& arg: args){
            args_evaled.push_back(arg->evaluate(interpreter));
        }


        CallablePtr func = as_func(callee_eval);

        if (args_evaled.size() != func->arity()){
            throw runtime_error("Expected " + std::to_string(func->arity()) + " arguments, got " + std::to_string(args_evaled.size()));
        }

        return func->call(interpreter, args_evaled);
    }
    // endregion

    // region AbstractInstAccessExpr
    AbstractInstAccessExpr::AbstractInstAccessExpr(const shared_ptr<Expr>& target, const Token& attr)
    : obj(target), attr_name(attr.get_lexeme()), attr_token(attr){
        if (attr.get_token_type() != TokenType::IDENTIFIER){
            throw parse_error(65, "An identifier is required for a GetAttr expression.");
        }
    }

    string AbstractInstAccessExpr::to_string() const{
        return obj->to_string() + "." + attr_name;
    }
    // endregion

    // region GetAttrExpr
    GetAttrExpr::GetAttrExpr(const shared_ptr<Expr>& target, const Token& attr): AbstractInstAccessExpr(target, attr){}

    string GetAttrExpr::to_string() const{
        return AbstractInstAccessExpr::to_string();
    }

    EvalResult GetAttrExpr::evaluate(const shared_ptr<Environment>& env){
        EvalResult object = obj->evaluate(env);
        if (holds_alternative<InstancePtr>(object)){
            return as_cls_inst(object)->get_attr(attr_name);
        }
        throw runtime_error("Can only access attributes from class instances.");
    }

    EvalResult GetAttrExpr::evaluate(const shared_ptr<Interpreter>& interpreter){
        EvalResult object = obj->evaluate(interpreter);
        if (holds_alternative<InstancePtr>(object)){
            return as_cls_inst(object)->get_attr(attr_name);
        }
        throw runtime_error("Can only access attributes from class instances.");
    }
    // endregion

    // region SetAttrExpr
    SetAttrExpr::SetAttrExpr(const shared_ptr<Expr>& target, const Token& attr, const shared_ptr<Expr>& value)
    : AbstractInstAccessExpr(target, attr), value(value){}

    string SetAttrExpr::to_string() const{
        return AbstractInstAccessExpr::to_string() + " = " + value->to_string();
    }

    EvalResult SetAttrExpr::evaluate(const shared_ptr<Environment>& env){
        EvalResult object = obj->evaluate(env);

        if (!holds_alternative<InstancePtr>(object)){
            throw runtime_error("Cannot access fields from non-instance values.");
        }

        EvalResult val = value->evaluate(env);
        as_cls_inst(object)->set_attr(attr_name, val);
        return val;
    }

    EvalResult SetAttrExpr::evaluate(const shared_ptr<Interpreter>& interpreter){
        EvalResult object = obj->evaluate(interpreter);

        if (!holds_alternative<InstancePtr>(object)){
            throw runtime_error("Cannot access fields from non-instance values.");
        }

        EvalResult val = value->evaluate(interpreter);
        as_cls_inst(object)->set_attr(attr_name, val);
        return val;
    }
    // endregion

    // region ThisExpr
    EvalResult ThisExpr::evaluate(const shared_ptr<Environment>& env){
        return env->get("this");
    }

    EvalResult ThisExpr::evaluate(const shared_ptr<Interpreter>& interpreter){
        return look_up_var(interpreter, "this", shared_from_this());
    }
    // endregion

    // region SuperExpr
    SuperExpr::SuperExpr(const Token& kw, const Token& meth)
    : kw(kw), meth(meth){
        if (kw.get_token_type() != TokenType::SUPER || meth.get_token_type() != TokenType::IDENTIFIER){
            throw parse_error(65, "Super expression built from wrong token type.");
        }
    }

    EvalResult SuperExpr::evaluate(const shared_ptr<Environment>& env){
        return "nil";
    }

    EvalResult SuperExpr::evaluate(const shared_ptr<Interpreter>& interpreter){
        auto shared = dynamic_pointer_cast<SuperExpr>(shared_from_this());
        size_t dist = get_super_dist(interpreter, shared);

        EvalResult cls_evaled = get_current_env(interpreter)->get_at(dist, "super");
        ClassPtr super_cls = dynamic_pointer_cast<LoxClass>(as_func(cls_evaled));
        InstancePtr obj = as_cls_inst(
            get_current_env(interpreter)->get_at(dist - 1, "this")
        );

        shared_ptr<LoxFunction> method = super_cls->find_meth(meth.get_lexeme());

        if (method == nullptr){
            throw runtime_error("Undefined property '" + meth.get_lexeme() + "'.");
        }

        return method->bind(obj);
    }
    // endregion

    // region ExprStatement
    void ExprStatement::execute(const shared_ptr<Environment>& env){
        EvalResult result = expr->evaluate(env);
    }

    void ExprStatement::execute(const shared_ptr<Interpreter>& interpreter){
        EvalResult result = expr->evaluate(interpreter);
    }
    // endregion

    // region PrintStatement
    void PrintStatement::execute(const shared_ptr<Environment>& env){
        EvalResult result = expr->evaluate(env);
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
        else if (holds_alternative<InstancePtr>(result)){
            cout << as_cls_inst(result)->to_string() << endl;
        }
        else{
            cout << as_string(result) << endl;
        }
    }

    void PrintStatement::execute(const shared_ptr<Interpreter>& interpreter){
        EvalResult result = expr->evaluate(interpreter);
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
        else if (holds_alternative<InstancePtr>(result)){
            cout << as_cls_inst(result)->to_string() << endl;
        }
        else{
            cout << as_string(result) << endl;
        }
    }
    // endregion

    // region VariableStatement
    VariableStatement::VariableStatement(const string& name, shared_ptr<Expr> init_expr): name(name), StatementWithExpr(init_expr){}  // NOLINT

    void VariableStatement::execute(const shared_ptr<Environment>& env){
        EvalResult val = "nil";
        if (expr != nullptr){
            val = expr->evaluate(env);
        }

        env->set(name, val);
    }

    void VariableStatement::execute(const shared_ptr<Interpreter>& interpreter){
        EvalResult val = "nil";
        if (expr != nullptr){
            val = expr->evaluate(interpreter);
        }

        get_current_env(interpreter)->set(name, val);
    }
    // endregion

    // region BlockStatement
    BlockStatement::BlockStatement(vector<shared_ptr<Statement>> statements)
            : statements(std::move(statements)){}

    void BlockStatement::execute(const shared_ptr<Environment>& env){
        shared_ptr<Environment> child_env = make_shared<Environment>(env);
        for (const auto& stmt: statements){
            stmt->execute(child_env);
        }
    }

    void BlockStatement::execute(const shared_ptr<Interpreter>& interpreter){
        add_nesting_level(interpreter);
        for (const auto& stmt: statements){
            stmt->execute(interpreter);
        }
        remove_nesting_level(interpreter);
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

    void IfStatement::execute(const shared_ptr<Environment>& env){
        EvalResult condit_evaled = condition->evaluate(env);
        if (is_truthy(condit_evaled)){
            return on_success->execute(env);
        }
        if (on_failure != nullptr){
            return on_failure->execute(env);
        }
    }

    void IfStatement::execute(const shared_ptr<Interpreter>& interpreter){
        EvalResult condit_evaled = condition->evaluate(interpreter);
        if (is_truthy(condit_evaled)){
            return on_success->execute(interpreter);
        }
        if (on_failure != nullptr){
            return on_failure->execute(interpreter);
        }
    }
    // endregion

    // region WhileStatement
    WhileStatement::WhileStatement(shared_ptr<Expr> condition, shared_ptr<Statement> success)
            : AbstractLogicalStmt(std::move(condition), std::move(success)){}

    void WhileStatement::execute(const shared_ptr<Environment>& env){
        while (is_truthy(condition->evaluate(env))){
            on_success->execute(env);
        }
    }

    void WhileStatement::execute(const shared_ptr<Interpreter>& interpreter){
        while (is_truthy(condition->evaluate(interpreter))){
            on_success->execute(interpreter);
        }
    }
    // endregion

    // region FunctionStmt
    FunctionStmt::FunctionStmt(const Token& id_token, const vector<Token>& args, const vector<shared_ptr<Statement>>& body)
    : args(args), body(body){
        name = id_token.get_lexeme();
        if (args.size() >= 255){
            throw invalid_argument("Cannot have 255 parameters or more in a function.");
        }
    }

    void FunctionStmt::execute(const shared_ptr<Environment>& env){
        shared_ptr<Statement> shared = shared_from_this();
        env->set(
            name,
            make_shared<LoxFunction>(shared, env, false)
        );
    }

    void FunctionStmt::execute(const shared_ptr<Interpreter>& interpreter){
        execute(get_current_env(interpreter));
    }

    namespace for_callable{
        EvalResult exec_func_body(const shared_ptr<Environment>& env, const shared_ptr<Statement>& func_stmt){
            auto as_func_stmt = dynamic_pointer_cast<FunctionStmt>(func_stmt);
            if (as_func_stmt == nullptr){
                return "nil";
            }

            try {
                for (const auto& stmt: as_func_stmt->body){
                    stmt->execute(env);
                }
                return "nil";
            }
            catch (const return_exc& returned_exc){
                return returned_exc.get_returned_val();
            }
        }

        EvalResult exec_func_body(const shared_ptr<Interpreter>& interpreter, const shared_ptr<Statement>& func_stmt){
            auto as_func_stmt = dynamic_pointer_cast<FunctionStmt>(func_stmt);
            if (as_func_stmt == nullptr){
                return "nil";
            }

            try {
                for (const auto& stmt: as_func_stmt->body){
                    stmt->execute(interpreter);
                }
                return "nil";
            }
            catch (const return_exc& returned_exc){
                return returned_exc.get_returned_val();
            }
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
    // endregion

    // region ReturnStmt
    void ReturnStmt::execute(const shared_ptr<Environment>& env){
        if (expr == nullptr){
            throw return_exc("nil");
        }
        throw return_exc(expr->evaluate(env));
    }

    void ReturnStmt::execute(const shared_ptr<Interpreter>& interpreter){
        if (expr == nullptr){
            throw return_exc("nil");
        }
        throw return_exc(expr->evaluate(interpreter));
    }
    // endregion

    // region ClassStmt
    ClassStmt::ClassStmt(const Token& id_token, const shared_ptr<VariableExpr>& superclass, const vector<shared_ptr<FunctionStmt>>& meths)
    : name(id_token.get_lexeme()), super_cls(superclass), methods(meths){
        if (id_token.get_token_type() != TokenType::IDENTIFIER){
            throw parse_error(65, "Classes must be declared using non-literal values.\0");
        }
    }

    void ClassStmt::execute(const shared_ptr<Environment>& env){
        auto current_env = env;
        EvalResult superclass = super_cls->evaluate(env);
        shared_ptr<LoxClass> supercls_as_cls = nullptr;
        if (!holds_alternative<CallablePtr>(superclass)){
            throw runtime_error("Superclass must be a class.");
        }
        else{
            CallablePtr as_callable = as_func(superclass);
            supercls_as_cls = dynamic_pointer_cast<LoxClass>(as_callable);
            if (supercls_as_cls == nullptr){
                throw runtime_error("Superclass must be a class.");
            }
            current_env = make_shared<Environment>(env);
            current_env->set("super", as_callable);
        }

        MethodMap meth_map;
        meth_map.reserve(methods.size());

        for (const auto& meth_decl: methods){
            meth_map.insert(
                {
                    meth_decl->get_name(),
                    make_shared<LoxFunction>(meth_decl, current_env, meth_decl->get_name() == "init")
                }
            );
        }

        env->set(
            name,
            make_shared<LoxClass>(name, supercls_as_cls, meth_map)
        );
    }

    void ClassStmt::execute(const shared_ptr<Interpreter>& interpreter){
        execute(get_current_env(interpreter));
    }
    // endregion
}
