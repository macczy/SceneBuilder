#pragma once
#include <memory>
#include "../Scanner/Token.h"
#include "Expression.h"

class Comparison
{
public:
	Comparison(const Position& position, Expression& expr1, Expression& expr2) :
		expr1(std::move(expr1)), expr2(std::move(expr2)), position(position) {};

	Expression& getFirstExpression() { return expr1; }
	Expression& getSecondExpression() { return expr2; }
	const Position& getPosition() const { return position; }
	virtual ~Comparison() {}
private:
	Expression expr1;
	Expression expr2;
	Position position;
};

class LessThan : public Comparison
{
public:
	LessThan(const Position& position, Expression& expr1, Expression& expr2) :
		Comparison(position, expr1, expr2) {};
	virtual ~LessThan() {}
};

class LessOrEqual : public Comparison
{
public:
	LessOrEqual(const Position& position, Expression& expr1, Expression& expr2) :
		Comparison(position, expr1, expr2) {};
	virtual ~LessOrEqual() {}
};

class GreaterThan : public Comparison
{
public:
	GreaterThan(const Position& position, Expression& expr1, Expression& expr2) :
		Comparison(position, expr1, expr2) {};
	virtual ~GreaterThan() {}
};

class GreaterOrEqual : public Comparison
{
public:
	GreaterOrEqual(const Position& position, Expression& expr1, Expression& expr2) :
		Comparison(position, expr1, expr2) {};
	virtual ~GreaterOrEqual() {}
};

class Equal : public Comparison
{
public:
	Equal(const Position& position, Expression& expr1, Expression& expr2) :
		Comparison(position, expr1, expr2) {};
	virtual ~Equal() {}
};

class NotEqual : public Comparison
{
public:
	NotEqual(const Position& position, Expression& expr1, Expression& expr2) :
		Comparison(position, expr1, expr2) {};
	virtual ~NotEqual() {}
};

namespace ComparisonFactory
{
	bool isComparisonOperator(const Token::TokenType& oper);
	std::unique_ptr<Comparison> getComparison(const Position& position, Expression& expr1, Expression& expr2, const Token::TokenType& oper);
};