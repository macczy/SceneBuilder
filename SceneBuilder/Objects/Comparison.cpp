#include <array>
#include "Comparison.h"


std::map<TokenType, std::function<ComparisonPtr(const Position& position, Expression& expr1, Expression& expr2)>> ComparisonFactory::generator {
	{TokenType::LESS_THAN, [](const Position& position, Expression& expr1, Expression& expr2) {
		return std::make_unique<LessThan>(position, expr1, expr2);
	}},
	{TokenType::LESS_OR_EQUAL, [](const Position& position, Expression& expr1, Expression& expr2) {
		return std::make_unique<LessOrEqual>(position, expr1, expr2);
	}},
	{TokenType::EQUAL_SIGN, [](const Position& position, Expression& expr1, Expression& expr2) {
		return std::make_unique<Equal>(position, expr1, expr2);
	}},
	{TokenType::NOT_EQUAL, [](const Position& position, Expression& expr1, Expression& expr2) {
		return std::make_unique<NotEqual>(position, expr1, expr2);
	}},
	{TokenType::GREATER_OR_EQUAL, [](const Position& position, Expression& expr1, Expression& expr2) {
		return std::make_unique<GreaterOrEqual>(position, expr1, expr2);
	}},
	{TokenType::GREATER_THAN, [](const Position& position, Expression& expr1, Expression& expr2) {
		return std::make_unique<GreaterThan>(position, expr1, expr2);
	}}
};

std::unique_ptr<Comparison> ComparisonFactory::getComparison(const Position& position, Expression& expr1, Expression& expr2, const TokenType& oper) {
	if (!isComparisonOperator(oper)) return nullptr;
	auto lambda = generator.find(oper);
	if (lambda == generator.end()) return nullptr;
	return lambda->second(position, expr1, expr2);
}

std::array<TokenType, 6> ComparisonFactory::tokens = {
		TokenType::LESS_THAN,
		TokenType::LESS_OR_EQUAL,
		TokenType::EQUAL_SIGN,
		TokenType::NOT_EQUAL,
		TokenType::GREATER_OR_EQUAL,
		TokenType::GREATER_THAN,
};

bool ComparisonFactory::isComparisonOperator(const TokenType& oper) {
	auto res = std::find(tokens.begin(), tokens.end(), oper);
	return res != tokens.end();
}