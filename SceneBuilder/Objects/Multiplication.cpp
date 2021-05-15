#include <array>
#include "Multiplication.h"

bool MultiplicationFactory::isMultiplicationOperator(const TokenType& oper) {
	std::array<TokenType, 2> tokens = {
	TokenType::ASTERISK,
	TokenType::SLASH
	};
	auto res = std::find(tokens.begin(), tokens.end(), oper);
	return res != tokens.end();

}
std::unique_ptr<Multiplication> MultiplicationFactory::getMultiplication(const Position& position, Expression& expr1, Expression& expr2, const TokenType& oper) {
	switch (oper) {
	case TokenType::ASTERISK:
		return std::make_unique<Multiplication_>(position, expr1, expr2);
	case TokenType::SLASH:
		return std::make_unique<Division>(position, expr1, expr2);
	default:
		return nullptr;
	}
}