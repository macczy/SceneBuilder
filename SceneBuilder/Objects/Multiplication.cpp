#include <array>
#include "Multiplication.h"

bool MultiplicationFactory::isMultiplicationOperator(const Token::TokenType& oper) {
	std::array<Token::TokenType, 2> tokens = {
	Token::TokenType::ASTERISK,
	Token::TokenType::SLASH
	};
	auto res = std::find(tokens.begin(), tokens.end(), oper);
	return res != tokens.end();

}
std::unique_ptr<Multiplication> MultiplicationFactory::getMultiplication(const Position& position, Expression& expr1, Expression& expr2, const Token::TokenType& oper) {
	switch (oper) {
	case Token::TokenType::PLUS:
		return std::make_unique<Multiplication_>(position, std::move(expr1), std::move(expr2));
	case Token::TokenType::MINUS:
		return std::make_unique<Division>(position, std::move(expr1), std::move(expr2));
	default:
		return nullptr;
	}
}