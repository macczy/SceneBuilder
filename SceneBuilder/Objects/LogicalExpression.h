#pragma once
#include <memory>
#include "../Scanner/Token.h"
#include "Comparison.h"

class LogicalExpression
{
protected:
	LogicalExpression(const Position& position, LogicalSubExpressionPtr& value1, LogicalSubExpressionPtr& value2) :
		value1(std::move(value1)), value2(std::move(value2)), position(position) {};

public:
	LogicalSubExpression* getFirstValue() { return value1.get(); }
	LogicalSubExpression* getSecondValue() { return value2.get(); }
	const Position& getPosition() const { return position; }
	virtual ~LogicalExpression() {}
private:
	LogicalSubExpressionPtr value1;
	LogicalSubExpressionPtr value2;
	Position position;
};

class DisjunctionExpression : public LogicalExpression
{
public:
	DisjunctionExpression(const Position& position, LogicalSubExpressionPtr& value1, LogicalSubExpressionPtr& value2) :
		LogicalExpression(position, value1, value2) {};
	virtual ~DisjunctionExpression() {}
};

class ConjuctionExpression : public LogicalExpression
{
public:
	ConjuctionExpression(const Position& position, LogicalSubExpressionPtr& value1, LogicalSubExpressionPtr& value2) :
		LogicalExpression(position, value1, value2) {};
	virtual ~ConjuctionExpression() {}

};

namespace LogicalExpressionFactory {
	bool isLogicalOperator(const Token::TokenType& oper);
}
const Position& getLogicalSubExpressionPosition(const LogicalSubExpression& val);