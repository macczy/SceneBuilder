#pragma once
#include <memory>
#include "../Scanner/Token.h"
#include "Value.h"
#include "Comparison.h"

class LogicalExpression;

using LogicalSubExpression = std::variant<std::unique_ptr<LogicalExpression>, std::unique_ptr<Comparison>>;

using ComparisonPtr = std::unique_ptr<Comparison>;
using LogicalExpressionPtr = std::unique_ptr<LogicalExpression>;

class LogicalExpression
{
public:
	LogicalExpression(const Position& position, std::unique_ptr<LogicalSubExpression> value1, std::unique_ptr<LogicalSubExpression>  value2) :
		value1(std::move(value1)), value2(std::move(value2)), position(position) {};

	LogicalSubExpression* getFirstValue() { return value1.get(); }
	LogicalSubExpression* getSecondValue() { return value2.get(); }
	const Position& getPosition() { return position; }
	virtual ~LogicalExpression() {}
private:
	std::unique_ptr<LogicalSubExpression> value1;
	std::unique_ptr<LogicalSubExpression> value2;
	Position position;
};

class DisjunctionExpression : public LogicalExpression
{
public:
	DisjunctionExpression(const Position& position, std::unique_ptr<LogicalSubExpression> value1, std::unique_ptr<LogicalSubExpression>  value2) :
		LogicalExpression(position, std::move(value1), std::move(value2)) {};
	virtual ~DisjunctionExpression() {}
};

class ConjuctionExpression : public LogicalExpression
{
public:
	ConjuctionExpression(const Position& position, std::unique_ptr<LogicalSubExpression> value1, std::unique_ptr<LogicalSubExpression>  value2) :
		LogicalExpression(position, std::move(value1), std::move(value2)) {};
	virtual ~ConjuctionExpression() {}

};

