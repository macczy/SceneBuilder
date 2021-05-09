#pragma once
#include <string>
#include "../Scanner/Token.h"
#include "LogicalExpression.h"
#include "Expression.h"

class TernaryExpression
{
public:
	TernaryExpression(const Position& position, LogicalExpressionPtr condition, Expression& ifTrue, Expression& ifFalse)
		: condition(std::move(condition)), expressionIfTrue(std::move(ifTrue)), expressionIfFalse(std::move(ifFalse)), position(position) {};
	const Expression& getTrueExpression() const { return expressionIfTrue; }
	const Expression& getFalseExpression() const { return expressionIfFalse; }
	const LogicalExpressionPtr& getCondition() const { return condition; }
	const Position& getPosition() const { return position; }
	~TernaryExpression() {};
private:
	Position position;
	LogicalExpressionPtr condition;
	Expression expressionIfTrue;
	Expression expressionIfFalse;
};