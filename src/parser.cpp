//
// Created by fortwoone on 31/03/2025.
//

#include "parser.hpp"


namespace lox::parser{

    // region Parser
    Parser::Parser(const vector<Token>& token_vec){
        tokens = token_vec;
    }

    Token Parser::advance(){
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
            Token op = previous();
            unique_ptr<ast::Expr> operand = get_unary();
            return make_unique<ast::UnaryExpr>(
                op,
                std::move(operand)
            );
        }

        return get_primary();
    }

    unique_ptr<ast::Expr> Parser::get_factor(){
        using enum TokenType;
        unique_ptr<ast::Expr> expr = get_unary();

        while (match({SLASH, STAR})){
            Token oper = previous();
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
            Token op = previous();
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
            Token oper = previous();
            unique_ptr<ast::Expr> right = get_term();
            expr = make_unique<ast::BinaryExpr>(
                std::move(expr),
                ast::_TOKEN_TO_OP.at(oper.get_token_type()),
                std::move(right)
            );
        }

        return expr;
    }

    unique_ptr<ast::Expr> Parser::get_equality(){
        using enum TokenType;
        unique_ptr<ast::Expr> expr = get_comparison();

        while (match({BANG_EQUAL, EQUAL_EQUAL})){
            Token oper = previous();
            unique_ptr<ast::Expr> right = get_comparison();
            expr = make_unique<ast::BinaryExpr>(
                std::move(expr),
                ast::_TOKEN_TO_OP.at(oper.get_token_type()),
                std::move(right)
            );
        }

        return expr;
    }

    unique_ptr<ast::Expr> Parser::get_expr(){
        return get_equality();
    }

    void Parser::parse(){
        unique_ptr<ast::Expr> expr = get_expr();
        cout << expr->to_string() << endl;
    }
    // endregion

    void parse(const string& file_contents) {
        bool contains_errors = false;

        vector<Token> tokens = tokenize(file_contents, &contains_errors);

        Parser parser = Parser(tokens);

        parser.parse();
    }
}
