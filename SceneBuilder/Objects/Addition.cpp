#include <array>
#include "Addition.h"

bool AdditionFactory::isAdditionOperator(const Token::TokenType& oper) {
	std::array<Token::TokenType, 2> tokens = {
	Token::TokenType::PLUS,
	Token::TokenType::MINUS
	};
	auto res = std::find(tokens.begin(), tokens.end(), oper);
	return res != tokens.end();
}
AdditionPtr AdditionFactory::getAddition(const Position& position, Expression& expr1, Expression& expr2, const Token::TokenType& oper) {
	switch (oper) {
	case Token::TokenType::PLUS:
		return std::make_unique<Sum>(position, std::move(expr1), std::move(expr2));
	case Token::TokenType::MINUS:
		return std::make_unique<Substraction>(position, std::move(expr1), std::move(expr2));
	default:
		return nullptr;
	}
}
