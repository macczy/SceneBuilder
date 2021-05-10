#pragma once
#include "../Scanner/Token.h"
#include "Expression.h"

class Multiplication {
public:
	Multiplication(const Position& position, Expression& expr1, Expression& expr2) :
		expr1(std::move(expr1)), expr2(std::move(expr2)), position(position) {};
	Expression& getFirstExpression() { return expr1; }
	Expression& getSecondExpression() { return expr2; }
	const Position& getPosition() const { return position; }
	virtual ~Multiplication() {};
private:
	Expression expr1;
	Expression expr2;
	Position position;
};

class Multiplication_ : public Multiplication
{
public:
	Multiplication_(const Position& position, Expression& expr1, Expression&  expr2) :
		Multiplication(position, expr1, expr2) {};

	virtual ~Multiplication_() {};
};

class Division : public Multiplication
{
public:
	Division(const Position& position, Expression& expr1, Expression& expr2) :
		Multiplication(position, expr1, expr2) {};
	virtual ~Division() {};
};

namespace MultiplicationFactory
{
	bool isMultiplicationOperator(const Token::TokenType& oper);
	std::unique_ptr<Multiplication> getMultiplication(const Position& position, Expression& expr1, Expression& expr2, const Token::TokenType& oper);
};