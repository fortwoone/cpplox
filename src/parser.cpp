//
// Created by fortwoone on 31/03/2025.
//

#include "parser.hpp"


namespace lox::parser{
    // region Parser
    Parser::Parser(vector<Token> token_vec): tokens(std::move(token_vec)), env{nullptr}, globals{nullptr}{}

    // Using const references to shared pointers to make absolutely SURE memory doesn't get out of hand when
    // handing a pointer to the environment object.
    Parser::Parser(vector<Token> token_vec, const shared_ptr<Environment>& env): tokens(std::move(token_vec)), env(env){
        globals = this->env;
    }

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
                    get_litexpr_tp_from_token_type(
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
            args,
            globals
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
                get_op_from_token(op.get_token_type()),
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
                    get_op_from_token(oper.get_token_type()),
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
                get_op_from_token(op.get_token_type()),
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
                    get_op_from_token(oper.get_token_type()),
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
                    get_op_from_token(oper.get_token_type()),
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
                get_op_from_token(op.get_token_type()),
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
                get_op_from_token(op.get_token_type()),
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

    StmtPtr Parser::get_func_decl(bool is_method){  // NOLINT
        using enum TokenType;
        Token name = consume(IDENTIFIER, is_method ? "Expected method name." : "Expected function name.");
        consume(LEFT_PAREN, is_method ? "Expected '(' after method name." : "Expected '(' after function name.");
        vector<Token> args;
        if (!check(RIGHT_PAREN)){
            do{
                if (args.size() >= 255){
                    throw parse_error(65, is_method ? "Cannot have more than 255 parameters in method definition." : "Cannot have more than 255 parameters in function definition.");
                }
                args.push_back(
                    consume(IDENTIFIER, "Expected parameter name.")
                );
            } while (match(COMMA));
        }
        consume(RIGHT_PAREN, "Expected ')' after parameter list.");

        consume(
            LEFT_BRACE,
            is_method ? "Expected '{' before method body." : "Expected '{' before function body."
        );

        vector<StmtPtr> body = get_block_stmt();
        return make_shared<ast::FunctionStmt>(
            name,
            args,
            body,
            globals
        );
    }

    StmtPtr Parser::get_declaration(){  // NOLINT
        if (match(TokenType::FUN)){
            return get_func_decl(false);
        }
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
