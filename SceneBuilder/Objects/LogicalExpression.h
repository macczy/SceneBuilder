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
	enum class Operator {
		OR,
		AND,
		VALUE
	};
	LogicalExpression(const Position& position, std::unique_ptr<LogicSubExpression> value1, std::unique_ptr<LogicSubExpression>  value2, const Operator& oper) :
		value1(std::move(value1)), value2(std::move(value2)), position(position), oper(oper) {};

	LogicSubExpression* getFirstValue() { return value1.get(); }
	LogicSubExpression* getSecondValue() { return value2.get(); }
	const Position& getPosition() { return position; }
	const Operator& getOperator() const { return oper; }
private:
	std::unique_ptr<LogicSubExpression> value1;
	std::unique_ptr<LogicSubExpression> value2;
	Position position;
	Operator oper;
};

class DisjunctionExpression : public LogicalExpression
{
public:
	DisjunctionExpression(const Position& position, std::unique_ptr<LogicSubExpression> value1, std::unique_ptr<LogicSubExpression>  value2) :
		LogicalExpression(position, std::move(value1), std::move(value2), Operator::OR) {};
};

class ConjuctionExpression : public LogicalExpression
{
public:
	ConjuctionExpression(const Position& position, std::unique_ptr<LogicSubExpression> value1, std::unique_ptr<LogicSubExpression>  value2) :
		LogicalExpression(position, std::move(value1), std::move(value2), Operator::AND) {};

};

