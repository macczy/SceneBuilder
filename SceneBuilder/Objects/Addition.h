#pragma once
#include <variant>
#include <memory>
#include "../Util/Token.h"
#include "../Util/ReturnType.h"
#include "Expression.h"

class Addition {
public:
	Addition(const Position& position, Expression& expr1, Expression& expr2) :
		expr1(std::move(expr1)), expr2(std::move(expr2)), position(position) {};
	Expression& getFirstExpression() { return expr1; }
	Expression& getSecondExpression() { return expr2; }
	const Position& getPosition() const { return position; }
	virtual ~Addition() {};
	ReturnType getReturnType(ReturnType first, ReturnType second);
	virtual std::map<std::pair<ReturnType, ReturnType>, ReturnType>& getAdditionResult() = 0;
	virtual std::string getOperator() const = 0;
private:
	Expression expr1;
	Expression expr2;
	Position position;
};

class Sum : public Addition {
public:
	Sum(const Position& position, Expression& expr1, Expression&  expr2) :
		Addition(position, expr1, expr2) {};

	virtual std::map<std::pair<ReturnType, ReturnType>, ReturnType>& getAdditionResult();
	virtual ~Sum() {};

	virtual std::string getOperator() const { return "+"; }
private:
	static std::map<std::pair<ReturnType, ReturnType>, ReturnType> sumResult;
};

class Substraction : public Addition {
public:
	Substraction(const Position& position, Expression& expr1, Expression& expr2) :
		Addition(position, expr1, expr2) {};
	virtual std::map<std::pair<ReturnType, ReturnType>, ReturnType>& getAdditionResult();
	virtual ~Substraction() {};
	virtual std::string getOperator() const { return "-"; }
private:
	static std::map<std::pair<ReturnType, ReturnType>, ReturnType> substractionResult;
};

namespace AdditionFactory {
	bool isAdditionOperator(const TokenType& oper);
	std::unique_ptr<Addition> getAddition(const Position& position, Expression& expr1, Expression& expr2, const TokenType& oper);
};