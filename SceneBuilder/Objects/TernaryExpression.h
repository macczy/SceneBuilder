#pragma once
#include <string>
#include "../Util/Token.h"
#include "LogicalExpression.h"
#include "Expression.h"

class TernaryExpression {
public:
	TernaryExpression(const Position& position, LogicalSubExpressionPtr& condition, Expression& ifTrue, Expression& ifFalse)
		: condition(std::move(condition)), expressionIfTrue(std::move(ifTrue)), expressionIfFalse(std::move(ifFalse)), position(position) {};
	const Expression& getTrueExpression() const { return expressionIfTrue; }
	const Expression& getFalseExpression() const { return expressionIfFalse; }
	const LogicalSubExpression* getCondition() const { return condition.get(); }
	const Position& getPosition() const { return position; }
	~TernaryExpression() {};
private:
	Position position;
	LogicalSubExpressionPtr condition;
	Expression expressionIfTrue;
	Expression expressionIfFalse;
};