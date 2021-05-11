#include <array>
#include "LogicalExpression.h"

bool LogicalExpressionFactory::isLogicalOperator(const Token::TokenType& oper) {
	std::array<Token::TokenType, 2> tokens = {
		Token::TokenType::OR,
		Token::TokenType::AND,
	};
	auto res = std::find(tokens.begin(), tokens.end(), oper);
	return res != tokens.end();
}

struct LogicalExpressionVisitor {
	const Position& operator()(const LogicalExpressionPtr& value) { return value->getPosition(); }
	const Position& operator()(const ComparisonPtr& value) { return value->getPosition(); }
};

const Position& getLogicalSubExpressionPosition(const LogicalSubExpression& val) {
	return std::visit(LogicalExpressionVisitor(), val);
}