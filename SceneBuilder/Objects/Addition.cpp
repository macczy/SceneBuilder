#include <array>
#include "Addition.h"

bool AdditionFactory::isAdditionOperator(const Token::TokenType& oper) {
	return oper == Token::TokenType::PLUS || oper == Token::TokenType::MINUS;
}
AdditionPtr AdditionFactory::getAddition(const Position& position, Expression& expr1, Expression& expr2, const Token::TokenType& oper) {
	switch (oper) {
	case Token::TokenType::PLUS:
		return std::make_unique<Sum>(position, expr1, expr2);
	case Token::TokenType::MINUS:
		return std::make_unique<Substraction>(position, expr1, expr2);
	default:
		return nullptr;
	}
}
