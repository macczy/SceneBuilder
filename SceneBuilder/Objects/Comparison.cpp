#include <array>
#include "Comparison.h"

std::unique_ptr<Comparison> ComparisonFactory::getComparison(const Position& position, std::unique_ptr<Value> value1, std::unique_ptr<Value> value2, const Token::TokenType& oper) {
	switch (oper) {
	case Token::TokenType::LESS_THAN:
		return std::make_unique<LessThan>(position, std::move(value1), std::move(value2));
	case Token::TokenType::LESS_OR_EQUAL:
		return std::make_unique < LessOrEqual>(position, std::move(value1), std::move(value2));
	case Token::TokenType::EQUAL_SIGN:
		return std::make_unique < Equal>(position, std::move(value1), std::move(value2));
	case Token::TokenType::NOT_EQUAL:
		return std::make_unique < NotEqual>(position, std::move(value1), std::move(value2));
	case Token::TokenType::GREATER_OR_EQUAL:
		return std::make_unique < GreaterOrEqual>(position, std::move(value1), std::move(value2));
	case Token::TokenType::GREATER_THAN:
		return std::make_unique < GreaterThan>(position, std::move(value1), std::move(value2));
	default:
		return nullptr;
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