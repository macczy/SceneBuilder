#include "Comparison.h"

Comparison ComparisonFactory::getComparison(const Position& position, std::unique_ptr<Value> value1, std::unique_ptr<Value> value2, const Token::TokenType& oper) {
	switch (oper) {
	case Token::TokenType::LESS_THAN:
		return LessThan(position, std::move(value1), std::move(value2));
	case Token::TokenType::LESS_OR_EQUAL:
		return LessOrEqual(position, std::move(value1), std::move(value2));
	case Token::TokenType::EQUAL_SIGN:
		return Equal(position, std::move(value1), std::move(value2));
	case Token::TokenType::NOT_EQUAL:
		return NotEqual(position, std::move(value1), std::move(value2));
	case Token::TokenType::GREATER_OR_EQUAL:
		return GreaterOrEqual(position, std::move(value1), std::move(value2));
	case Token::TokenType::GREATER_THAN:
		return GreaterThan(position, std::move(value1), std::move(value2));
	default:
		throw std::runtime_error("Undefined comparison operator");
	}
}

bool ComparisonFactory::isComparisonOperator(const Token::TokenType& oper) {
	std::array<Token::TokenType, 6> tokens = {
		Token::TokenType::LESS_THAN,
		Token::TokenType::LESS_OR_EQUAL,
		Token::TokenType::EQUAL_SIGN,
		Token::TokenType::NOT_EQUAL,
		Token::TokenType::GREATER_OR_EQUAL,
		Token::TokenType::GREATER_THAN,
	};
	auto res = std::find(tokens.begin(), tokens.end(), oper);
	return res != tokens.end();
}