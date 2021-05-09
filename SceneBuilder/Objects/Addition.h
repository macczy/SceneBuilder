#pragma once
#include <variant>
#include <memory>
#include "../Scanner/Token.h"
#include "Expression.h"

class Addition {
public:
	Addition(const Position& position, Expression expr1, Expression expr2) :
		expr1(std::move(expr1)), expr2(std::move(expr2)), position(position) {};
	Expression& getFirstExpression() { return expr1; }
	Expression& getSecondExpression() { return expr2; }
	const Position& getPosition() const { return position; }
	virtual ~Addition() {};
private:
	Expression expr1;
	Expression expr2;
	Position position;
};

class Sum : public Addition
{
public:
	Sum(const Position& position, Expression expr1, Expression  expr2) :
		Addition(position, std::move(expr1), std::move(expr2)) {};

	virtual ~Sum() {};
};

class Substraction : public Addition
{
public:
	Substraction(const Position& position, Expression expr1, Expression expr2) :
		Addition(position, std::move(expr1), std::move(expr2)) {};
	virtual ~Substraction() {};
};

namespace AdditionFactory
{
	bool isAdditionOperator(const Token::TokenType& oper);
	std::unique_ptr<Addition> getAddition(const Position& position, Expression& expr1, Expression& expr2, const Token::TokenType& oper);
};