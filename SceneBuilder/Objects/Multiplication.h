#pragma once
#include "../Util/Token.h"
#include "../Util/ReturnType.h"
#include "Expression.h"

class Multiplication {
public:
	Multiplication(const Position& position, Expression& expr1, Expression& expr2) :
		expr1(std::move(expr1)), expr2(std::move(expr2)), position(position) {};
	Expression& getFirstExpression() { return expr1; }
	Expression& getSecondExpression() { return expr2; }
	const Position& getPosition() const { return position; }
	virtual ~Multiplication() {};
	virtual std::string getOperator() const = 0;
	ReturnType getReturnType(ReturnType first, ReturnType second);
protected:
	virtual std::map<std::pair<ReturnType, ReturnType>, ReturnType>& getMultiplicationResult() = 0;
private:
	Expression expr1;
	Expression expr2;
	Position position;
};

class Multiplication_ : public Multiplication {
public:
	Multiplication_(const Position& position, Expression& expr1, Expression&  expr2) :
		Multiplication(position, expr1, expr2) {};
	virtual ~Multiplication_() {};
	virtual std::string getOperator() const { return "*"; }
protected:
	virtual std::map<std::pair<ReturnType, ReturnType>, ReturnType>& getMultiplicationResult();
private:
	static std::map<std::pair<ReturnType, ReturnType>, ReturnType> multiplicationResult;
};

class Division : public Multiplication {
public:
	Division(const Position& position, Expression& expr1, Expression& expr2) :
		Multiplication(position, expr1, expr2) {};
	virtual ~Division() {};
	virtual std::string getOperator() const { return "/"; }
protected:
	virtual std::map<std::pair<ReturnType, ReturnType>, ReturnType>& getMultiplicationResult();
private:
	static std::map<std::pair<ReturnType, ReturnType>, ReturnType> divisionResult;
};

namespace MultiplicationFactory {
	bool isMultiplicationOperator(const TokenType& oper);
	std::unique_ptr<Multiplication> getMultiplication(const Position& position, Expression& expr1, Expression& expr2, const TokenType& oper);
};