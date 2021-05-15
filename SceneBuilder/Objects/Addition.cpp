#include <array>
#include "Addition.h"

bool AdditionFactory::isAdditionOperator(const TokenType& oper) {
	return oper == TokenType::PLUS || oper == TokenType::MINUS;
}
AdditionPtr AdditionFactory::getAddition(const Position& position, Expression& expr1, Expression& expr2, const TokenType& oper) {
	switch (oper) {
	case TokenType::PLUS:
		return std::make_unique<Sum>(position, expr1, expr2);
	case TokenType::MINUS:
		return std::make_unique<Substraction>(position, expr1, expr2);
	default:
		return nullptr;
	}
}
