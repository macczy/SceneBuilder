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
std::unique_ptr<Addition> AdditionFactory::getAddition(const Position& position, std::unique_ptr<Term> value1, std::unique_ptr<Term> value2, const Token::TokenType& oper) {
	switch (oper) {
	case Token::TokenType::PLUS:
		return std::make_unique<Sum>(position, std::move(value1), std::move(value2));
	case Token::TokenType::MINUS:
		return std::make_unique<Substraction>(position, std::move(value1), std::move(value2));
	default:
		return nullptr;
	}
}