//
// Created by fortwoone on 31/03/2025.
//

#include "parser.hpp"


namespace lox::parser{
#   define as_double get<double>
#   define as_bool get<bool>
#   define as_string get<string>

    namespace ast{
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

        // region BinaryExpr
        string BinaryExpr::to_string() const{
            return "(" + _OP_TO_SYM.at(op) + " " + left->to_string() + " " + right->to_string() + ")";
        }

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
                    break;
                case GREATER:
                    if (two_numbers){
                        return as_double(left_result) > as_double(right_result);
                    }
                    break;
                case LESS_EQUAL:
                    if (two_numbers){
                        return as_double(left_result) <= as_double(right_result);
                    }
                    break;
                case GREATER_EQUAL:
                    if (two_numbers){
                        return as_double(left_result) >= as_double(right_result);
                    }
                    break;
                default:
                    break;
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
    }

    // region Parser
    Parser::Parser(vector<Token>& token_vec): tokens(std::move(token_vec)){}

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

    unique_ptr<ast::Expr> Parser::get_primary(){
        using enum TokenType;
        using ast::LiteralExprType;
        if (match(FALSE)){
            return make_unique<ast::LiteralExpr>(LiteralExprType::FALSE);
        }
        if (match(TRUE)){
            return make_unique<ast::LiteralExpr>(LiteralExprType::TRUE);
        }
        if (match(NIL)){
            return make_unique<ast::LiteralExpr>(LiteralExprType::NIL);
        }

        if (match({NUMBER, STRING})){
            return make_unique<ast::LiteralExpr>(
                    ast::_TOKEN_TO_LITEXPRTP.at(
                            previous().get_token_type()
                    ),
                    previous().get_literal_formatted_value()
            );
        }

        if (match(LEFT_PAREN)){
            unique_ptr<ast::Expr> ptr = get_expr();
            consume(RIGHT_PAREN, "Expected ')' after expression.");
            return make_unique<ast::GroupExpr>(std::move(ptr));
        }

        throw invalid_argument("No match was made.");
    }

    unique_ptr<ast::Expr> Parser::get_unary(){
        using enum TokenType;
        if (match({BANG, MINUS})){
            Token& op = previous();
            unique_ptr<ast::Expr> operand = get_unary();
            return make_unique<ast::UnaryExpr>(
                ast::_TOKEN_TO_OP.at(op.get_token_type()),
                std::move(operand)
            );
        }

        return get_primary();
    }

    unique_ptr<ast::Expr> Parser::get_factor(){
        using enum TokenType;
        unique_ptr<ast::Expr> expr = get_unary();

        while (match({SLASH, STAR})){
            Token& oper = previous();
            unique_ptr<ast::Expr> right = get_unary();
            expr = make_unique<ast::BinaryExpr>(
                    std::move(expr),
                    ast::_TOKEN_TO_OP.at(oper.get_token_type()),
                    std::move(right)
            );
        }

        return expr;
    }

    unique_ptr<ast::Expr> Parser::get_term(){
        using enum TokenType;
        unique_ptr<ast::Expr> expr = get_factor();

        while (match({MINUS, PLUS})){
            Token& op = previous();
            unique_ptr<ast::Expr> right = get_factor();
            expr = make_unique<ast::BinaryExpr>(
                std::move(expr),
                ast::_TOKEN_TO_OP.at(op.get_token_type()),
                std::move(right)
            );
        }

        return expr;
    }

    unique_ptr<ast::Expr> Parser::get_comparison(){
        using enum TokenType;
        unique_ptr<ast::Expr> expr = get_term();

        while (match({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})){
            Token& oper = previous();
            try{
                unique_ptr<ast::Expr> right = get_term();
                expr = make_unique<ast::BinaryExpr>(
                    std::move(expr),
                    ast::_TOKEN_TO_OP.at(oper.get_token_type()),
                    std::move(right)
                );
            } catch(const invalid_argument& exc){
                throw exc;
            }
        }

        return expr;
    }

    unique_ptr<ast::Expr> Parser::get_equality(){
        using enum TokenType;
        unique_ptr<ast::Expr> expr = get_comparison();

        while (match({BANG_EQUAL, EQUAL_EQUAL})){
            Token& oper = previous();
            try{
                unique_ptr<ast::Expr> right = get_comparison();
                expr = make_unique<ast::BinaryExpr>(
                    std::move(expr),
                    ast::_TOKEN_TO_OP.at(oper.get_token_type()),
                    std::move(right)
                );
            }
            catch (const invalid_argument& exc){
                cerr << "Error: expected expression after operator." << endl;
                throw exc;
            }
        }

        return expr;
    }

    unique_ptr<ast::Expr> Parser::get_expr(){
        try{
            return get_equality();
        }
        catch (const invalid_argument& exc){
            throw exc;
        }
    }

    unique_ptr<ast::Expr> Parser::parse(){
        unique_ptr<ast::Expr> expr = get_expr();
        return std::move(expr);
    }

    ubyte Parser::evaluate(){
        try{
            unique_ptr<ast::Expr> expr = std::move(parse());
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

    unique_ptr<ast::Expr> parse(const string& file_contents) {
        bool contains_errors = false;

        vector<Token> tokens = tokenize(file_contents, &contains_errors);
        if (contains_errors){
            throw parse_error(65, "Error while parsing (tokenising stage)\0");
        }

        Parser parser = Parser(tokens);

        try{
            unique_ptr<ast::Expr> expr = std::move(parser.parse());
            return std::move(expr);
        }
        catch (const invalid_argument& exc){
            contains_errors = true;
            throw parse_error(65, exc.what());
        }
    }

    ubyte evaluate(const string& file_contents){
        try{
            bool contains_errors = true;
            vector<Token> tokens = tokenize(file_contents, &contains_errors);
            if (contains_errors){
                throw parse_error(65, "Error while parsing (tokenising stage)\0");
            }

            Parser parser = Parser(tokens);
            return parser.evaluate();
        }
        catch (const parse_error& exc){
            cerr << exc.what() << endl;
            return exc.get_return_code();
        }
    }
}
