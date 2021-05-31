#pragma once
#include <memory>
#include "../Util/Token.h"
#include "Comparison.h"

class LogicalExpression {
protected:
	LogicalExpression(const Position& position, LogicalSubExpressionPtr& value1, LogicalSubExpressionPtr& value2) :
		value1(std::move(value1)), value2(std::move(value2)), position(position) {};

public:
	LogicalSubExpression* getFirstValue() { return value1.get(); }
	LogicalSubExpression* getSecondValue() { return value2.get(); }
	const Position& getPosition() const { return position; }
	virtual ~LogicalExpression() {}
	virtual std::string getOperator() const = 0;
private:
	LogicalSubExpressionPtr value1;
	LogicalSubExpressionPtr value2;
	Position position;
};

class DisjunctionExpression : public LogicalExpression {
public:
	DisjunctionExpression(const Position& position, LogicalSubExpressionPtr& value1, LogicalSubExpressionPtr& value2) :
		LogicalExpression(position, value1, value2) {};
	virtual ~DisjunctionExpression() {}

	virtual std::string getOperator() const { return " || "; }
};

class ConjuctionExpression : public LogicalExpression {
public:
	ConjuctionExpression(const Position& position, LogicalSubExpressionPtr& value1, LogicalSubExpressionPtr& value2) :
		LogicalExpression(position, value1, value2) {};
	virtual ~ConjuctionExpression() {}
	virtual std::string getOperator() const { return " && "; }

};

namespace LogicalExpressionFactory {
	bool isLogicalOperator(const TokenType& oper);
}
const Position& getLogicalSubExpressionPosition(const LogicalSubExpression& val);