#pragma once
#include <memory>
#include "../Scanner/Token.h"
#include "Value.h"
#include "Comparison.h"

class LogicalExpression;

using LogicSubExpression = std::variant<LogicalExpression, Comparison>;

class LogicalExpression
{
public:
	LogicalExpression(const Position& position, std::unique_ptr<LogicSubExpression> value1, std::unique_ptr<LogicSubExpression>  value2) :
		value1(std::move(value1)), value2(std::move(value2)), position(position) {};

	LogicSubExpression* getFirstValue() { return value1.get(); }
	LogicSubExpression* getSecondValue() { return value2.get(); }
	const Position& getPosition() { return position; }
	virtual ~LogicalExpression() {}
private:
	std::unique_ptr<LogicSubExpression> value1;
	std::unique_ptr<LogicSubExpression> value2;
	Position position;
};

class DisjunctionExpression : public LogicalExpression
{
public:
	DisjunctionExpression(const Position& position, std::unique_ptr<LogicSubExpression> value1, std::unique_ptr<LogicSubExpression>  value2) :
		LogicalExpression(position, std::move(value1), std::move(value2)) {};
	virtual ~DisjunctionExpression() {}
};

class ConjuctionExpression : public LogicalExpression
{
public:
	ConjuctionExpression(const Position& position, std::unique_ptr<LogicSubExpression> value1, std::unique_ptr<LogicSubExpression>  value2) :
		LogicalExpression(position, std::move(value1), std::move(value2)) {};
	virtual ~ConjuctionExpression() {}

};

