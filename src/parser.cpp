//
// Created by fortwoone on 31/03/2025.
//

#include "parser.hpp"


namespace lox::parser{
#   define as_double get<double>
#   define as_bool get<bool>
#   define as_string get<string>

    namespace ast{
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


            CallablePtr func = get<CallablePtr>(callee_eval);

            if (args_evaled.size() != func->arity()){
                throw runtime_error("Expected " + std::to_string(func->arity()) + " arguments, got " + std::to_string(args_evaled.size()));
            }

            return func->call(args_evaled);
        }
        // endregion

        // region ExprStatement
        void ExprStatement::execute() const{
            EvalResult result = expr->evaluate();
        }
        // endregion

        // region PrintStatement
        void PrintStatement::execute() const{
            EvalResult result = expr->evaluate();
            if (holds_alternative<bool>(result)){
                cout << (as_bool(result) ? "true" : "false") << endl;
            }
            else if (holds_alternative<double>(result)){
                if (static_cast<int>(as_double(result) == as_double(result))){
                    cout << fixed << setprecision(0);
                }
                cout << as_double(result) << endl;
                cout << defaultfloat;
            }
            else{
                cout << as_string(result) << endl;
            }
        }
        // endregion

        // region VariableStatement
        VariableStatement::VariableStatement(const string& name, shared_ptr<Expr> init_expr): name(name), StatementWithExpr(init_expr){}  // NOLINT

        void VariableStatement::execute() const{
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

        void BlockStatement::execute() const{
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

        void IfStatement::execute() const{
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

        void WhileStatement::execute() const{
            while (is_truthy(condition->evaluate())){
                on_success->execute();
            }
        }
        // endregion
    }

    // region Parser
    Parser::Parser(vector<Token> token_vec): tokens(std::move(token_vec)), env{nullptr}{}

    // Using const references to shared pointers to make absolutely SURE memory doesn't get out of hand when
    // handing a pointer to the environment object.
    Parser::Parser(vector<Token> token_vec, const shared_ptr<Environment>& env): tokens(std::move(token_vec)), env(env){}

    Token& Parser::advance(){
        if (!is_at_end()){
            current_idx++;
        }
        return previous();
    }

    bool Parser::check(TokenType type){
        if (is_at_end()){
            return false;
        }
        return peek().get_token_type() == type;
    }

    bool Parser::match(initializer_list<TokenType> token_types){
        bool ret = any_of(
            token_types.begin(),
            token_types.end(),
            [this](TokenType tp){
                return check(tp);
            }
        );
        if (ret){
            advance();
        }
        return ret;
    }

    ExprPtr Parser::get_primary(){  // NOLINT
        using enum TokenType;
        using ast::LiteralExprType;
        if (match(FALSE)){
            return make_shared<ast::LiteralExpr>(LiteralExprType::FALSE);
        }
        if (match(TRUE)){
            return make_shared<ast::LiteralExpr>(LiteralExprType::TRUE);
        }
        if (match(NIL)){
            return make_shared<ast::LiteralExpr>(LiteralExprType::NIL);
        }

        if (match(IDENTIFIER)){
            return make_shared<ast::VariableExpr>(previous(), env);
        }

        if (match({NUMBER, STRING})){
            return make_shared<ast::LiteralExpr>(
                    ast::_TOKEN_TO_LITEXPRTP.at(
                            previous().get_token_type()
                    ),
                    previous().get_literal_formatted_value()
            );
        }

        if (match(LEFT_PAREN)){
            ExprPtr ptr = get_expr();
            consume(RIGHT_PAREN, "Expected ')' after expression.");
            return make_shared<ast::GroupExpr>(ptr);
        }

        throw parse_error(65, "There is no literal to parse.");
    }

    ExprPtr Parser::get_call_end(const ExprPtr& callee){  // NOLINT
        using enum TokenType;

        vector<ExprPtr> args;
        if (!check(RIGHT_PAREN)){
            do {
                if (args.size() >= 255){
                    throw parse_error(65, "Cannot have more than 255 arguments in a function.");
                }
                args.push_back(get_expr());
            } while (match(COMMA));
        }

        Token& paren = consume(RIGHT_PAREN, "Expected ')' after arguments.");

        return make_shared<ast::CallExpr>(
            callee,
            paren,
            args
        );
    }

    ExprPtr Parser::get_call(){  // NOLINT
        ExprPtr expr = get_primary();
        while (true){
            if (match(TokenType::LEFT_PAREN)){
                expr = get_call_end(expr);
            }
            else{
                break;
            }
        }
        return expr;
    }

    ExprPtr Parser::get_unary(){  // NOLINT
        using enum TokenType;
        if (match({BANG, MINUS})){
            Token& op = previous();
            ExprPtr operand = get_unary();
            return make_shared<ast::UnaryExpr>(
                ast::_TOKEN_TO_OP.at(op.get_token_type()),
                operand
            );
        }

        return get_call();
    }

    ExprPtr Parser::get_factor(){  // NOLINT
        using enum TokenType;
        ExprPtr expr = get_unary();

        while (match({SLASH, STAR})){
            Token& oper = previous();
            ExprPtr right = get_unary();
            expr = make_shared<ast::BinaryExpr>(
                    expr,
                    ast::_TOKEN_TO_OP.at(oper.get_token_type()),
                    right
            );
        }

        return expr;
    }

    ExprPtr Parser::get_term(){  // NOLINT
        using enum TokenType;
        ExprPtr expr = get_factor();

        while (match({MINUS, PLUS})){
            Token& op = previous();
            ExprPtr right = get_factor();
            expr = make_shared<ast::BinaryExpr>(
                expr,
                ast::_TOKEN_TO_OP.at(op.get_token_type()),
                right
            );
        }

        return expr;
    }

    ExprPtr Parser::get_comparison(){  // NOLINT
        using enum TokenType;
        ExprPtr expr = get_term();

        while (match({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})){
            Token& oper = previous();
            try{
                ExprPtr right = get_term();
                expr = make_shared<ast::BinaryExpr>(
                    expr,
                    ast::_TOKEN_TO_OP.at(oper.get_token_type()),
                    right
                );
            }
            catch (const invalid_argument& exc){
                throw exc;
            }
        }

        return expr;
    }

    ExprPtr Parser::get_equality(){  // NOLINT
        using enum TokenType;
        ExprPtr expr = get_comparison();

        while (match({BANG_EQUAL, EQUAL_EQUAL})){
            Token& oper = previous();
            try{
                ExprPtr right = get_comparison();
                expr = make_shared<ast::BinaryExpr>(
                    expr,
                    ast::_TOKEN_TO_OP.at(oper.get_token_type()),
                    right
                );
            }
            catch (const invalid_argument& exc){
                cerr << "Error: expected expression after operator." << endl;
                throw exc;
            }
        }

        return expr;
    }

    ExprPtr Parser::get_and(){  // NOLINT
        ExprPtr expr = get_equality();

        while (match(TokenType::AND)){
            Token& op = previous();
            ExprPtr right = get_equality();
            expr = make_shared<ast::LogicalExpr>(
                std::move(expr),
                ast::_TOKEN_TO_OP.at(op.get_token_type()),
                std::move(right)
            );
        }

        return expr;
    }

    ExprPtr Parser::get_or(){  // NOLINT
        ExprPtr expr = get_and();

        while (match(TokenType::OR)){
            Token& op = previous();
            ExprPtr right = get_and();
            expr = make_shared<ast::LogicalExpr>(
                std::move(expr),
                ast::_TOKEN_TO_OP.at(op.get_token_type()),
                std::move(right)
            );
        }

        return expr;
    }

    ExprPtr Parser::get_assignment(){  // NOLINT
        using ast::VariableExpr;
        ExprPtr expr = get_or();

        if (match(TokenType::EQUAL)){
            Token& equals = previous();
            ExprPtr value = get_assignment();

            auto as_var_expr = dynamic_pointer_cast<VariableExpr>(expr);
            if (as_var_expr != nullptr){
                return make_shared<ast::AssignmentExpr>(
                    as_var_expr->get_name(),
                    value,
                    env
                );
            }

            throw parse_error(65, "Invalid assignment target.");
        }

        return expr;
    }

    ExprPtr Parser::get_expr(){  // NOLINT
        try{
            return get_assignment();
        }
        catch (const invalid_argument& exc){
            throw exc;
        }
    }

    ExprPtr Parser::parse_old(){
        ExprPtr expr = get_expr();
        return expr;
    }

    StmtPtr Parser::get_print_statement(){
        ExprPtr val = get_expr();
        consume(TokenType::SEMICOLON, "Expected ';' after value.");
        return make_shared<ast::PrintStatement>(
            val
        );
    }

    StmtPtr Parser::get_expr_statement(){
        ExprPtr val = get_expr();
        consume(TokenType::SEMICOLON, "Expected ';' after expression.");
        return make_shared<ast::ExprStatement>(val);
    }

    vector<StmtPtr> Parser::get_block_stmt(){  // NOLINT
        vector<StmtPtr> ret;
        while (!check(TokenType::RIGHT_BRACE) && !is_at_end()){
            ret.emplace_back(get_declaration());
        }

        consume(TokenType::RIGHT_BRACE, "Expected '}' after block.");
        return ret;
    }

    StmtPtr Parser::get_while_stmt(){  // NOLINT
        consume(TokenType::LEFT_PAREN, "Expected '(' after 'while'.");
        ExprPtr condition = get_expr();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after condition.");
        StmtPtr body = get_statement();

        return make_shared<ast::WhileStatement>(
            condition,
            body
        );
    }

    StmtPtr Parser::get_if_statement(){  // NOLINT
        using enum TokenType;
        consume(LEFT_PAREN, "Expected '(' after 'if' keyword.");
        ExprPtr condition = get_expr();
        consume(RIGHT_PAREN, "Expected ')' after condition.");

        StmtPtr success_branch = get_statement();
        StmtPtr failure_branch = nullptr;
        if (match(ELSE)){
            failure_branch = get_statement();
            return make_shared<ast::IfStatement>(
                condition,
                success_branch,
                failure_branch
            );
        }

        return make_shared<ast::IfStatement>(
            condition,
            success_branch
        );
    }

    StmtPtr Parser::get_for_statement(){  // NOLINT
        // Instead of making a separate statement class,
        // we just convert for loops into while loops.
        using enum TokenType;

        consume(LEFT_PAREN, "Expected '(' after 'for' keyword.");

        StmtPtr initialiser;
        ExprPtr condition = nullptr, increment = nullptr;

        // Initialiser.
        if (match(SEMICOLON)){
            initialiser = nullptr;
        }
        else if (match(VAR)){
            // If there actually is an initialiser, we need one nesting level.
            // Doing it now so the next statements use the correct environment to look variables up into.
            env = make_shared<Environment>(env);
            initialiser = get_var_declaration();
        }
        else{
            // Same as comment above.
            env = make_shared<Environment>(env);
            initialiser = get_expr_statement();
        }

        if (!check(SEMICOLON)){
            condition = get_expr();  // Otherwise, there is no condition.
        }
        consume(SEMICOLON, "Expected ';' after loop condition.");

        if (!check(RIGHT_PAREN)){
            // We also need an additional nested environment if there is an incrementer. In that case,
            // we need it to be one level deeper, due to the block statement (incrementer) being contained into
            // another block statement (the one with the body and initialiser).
            env = make_shared<Environment>(env);
            increment = get_expr();
        }
        consume(RIGHT_PAREN, "Expected ')' after for clauses.");

        StmtPtr body = get_statement();

        if (increment != nullptr){
            // Insert the increment at the end of the existing body.
            vector<StmtPtr> as_block;
            as_block.reserve(2);
            as_block.push_back(body);
            as_block.push_back(
                make_shared<ast::ExprStatement>(increment)
            );
            body = make_shared<ast::BlockStatement>(as_block, env);
            env = env->get_enclosing();
        }

        if (condition == nullptr){
            // Consider the condition as true for the computed "while" loop if none was provided.
            condition = make_shared<ast::LiteralExpr>(
                ast::LiteralExprType::TRUE
            );
        }
        body = make_shared<ast::WhileStatement>(
            condition,
            body
        );

        if (initialiser != nullptr){
            vector<StmtPtr> as_init_block;
            as_init_block.reserve(2);
            as_init_block.push_back(initialiser);
            as_init_block.push_back(body);
            body = make_shared<ast::BlockStatement>(as_init_block, env);
            env = env->get_enclosing();
        }

        while (env->get_enclosing() != nullptr){
            env = env->get_enclosing();
        }

        return body;
    }

    StmtPtr Parser::get_statement(){  // NOLINT
        using enum TokenType;
        if (match(FOR)){
            return get_for_statement();
        }
        if (match(IF)){
            return get_if_statement();
        }
        if (match(WHILE)){
            return get_while_stmt();
        }
        if (match(PRINT)){
            return get_print_statement();
        }
        if (match(LEFT_BRACE)){
            try{
                env = make_shared<Environment>(env);
                auto ret = make_shared<ast::BlockStatement>(
                    get_block_stmt(),
                    env
                );
                env = env->get_enclosing();
                return ret;
            }
            catch (const invalid_argument& exc){
                env = env->get_enclosing();
                throw exc;
            }
            catch (const parse_error& exc){
                env = env->get_enclosing();
                throw exc;
            }
        }
        return get_expr_statement();
    }

    StmtPtr Parser::get_var_declaration(){
        Token& name = consume(TokenType::IDENTIFIER, "Expected a variable name.");

        ExprPtr init = nullptr;
        if (match(TokenType::EQUAL)){
            init = get_expr();
        }

        consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");
        return make_shared<ast::VariableStatement>(
            name.get_lexeme(),
            init
        );
    }

    StmtPtr Parser::get_declaration(){  // NOLINT
        if (match(TokenType::VAR)){
            return get_var_declaration();
        }
        return get_statement();
    }

    vector<StmtPtr> Parser::parse(){
        vector<StmtPtr> statements;
        while (!is_at_end()){
            statements.push_back(
                get_declaration()
            );
        }
        return statements;
    }

    ubyte Parser::evaluate(){
        try{
            ExprPtr expr = parse_old();
            auto result = expr->evaluate();
            if (holds_alternative<double>(result)){
                cout << as_double(result) << endl;
            }
            else if (holds_alternative<bool>(result)){
                cout << (as_bool(result) ? "true" : "false") << endl;
            }
            else{
                cout << as_string(result) << endl;
            }
            return 0;
        }
        catch (const parse_error& exc){
            cerr << exc.what() << endl;
            return exc.get_return_code();
        }
    }
    // endregion

    ExprPtr parse(const string& file_contents) {
        bool contains_errors = false;

        Parser parser = Parser(tokenize(file_contents, &contains_errors));

        try{
            ExprPtr expr = parser.parse_old();
            return expr;
        }
        catch (const invalid_argument& exc){
            contains_errors = true;
            throw parse_error(65, exc.what());
        }
    }

    ubyte evaluate(const string& file_contents){
        try{
            bool contains_errors = true;

            Parser parser = Parser(tokenize(file_contents, &contains_errors));
            return parser.evaluate();
        }
        catch (const parse_error& exc){
            cerr << exc.what() << endl;
            return exc.get_return_code();
        }
    }
}
