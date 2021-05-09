#include <array>
#include "Comparison.h"

std::unique_ptr<Comparison> ComparisonFactory::getComparison(const Position& position, Expression& expr1, Expression& expr2, const Token::TokenType& oper) {
	switch (oper) {
	case Token::TokenType::LESS_THAN:
		return std::make_unique<LessThan>(position, std::move(expr1), std::move(expr2));
	case Token::TokenType::LESS_OR_EQUAL:
		return std::make_unique < LessOrEqual>(position, std::move(expr1), std::move(expr2));
	case Token::TokenType::EQUAL_SIGN:
		return std::make_unique < Equal>(position, std::move(expr1), std::move(expr2));
	case Token::TokenType::NOT_EQUAL:
		return std::make_unique < NotEqual>(position, std::move(expr1), std::move(expr2));
	case Token::TokenType::GREATER_OR_EQUAL:
		return std::make_unique < GreaterOrEqual>(position, std::move(expr1), std::move(expr2));
	case Token::TokenType::GREATER_THAN:
		return std::make_unique < GreaterThan>(position, std::move(expr1), std::move(expr2));
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