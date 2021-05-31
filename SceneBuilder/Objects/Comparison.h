#pragma once
#include <memory>
#include <functional>
#include <map>
#include <array>
#include "../Util/Token.h"
#include "Expression.h"

class Comparison {
public:
	Comparison(const Position& position, Expression& expr1, Expression& expr2) :
		expr1(std::move(expr1)), expr2(std::move(expr2)), position(position) {};

	Expression& getFirstExpression() { return expr1; }
	Expression& getSecondExpression() { return expr2; }
	const Position& getPosition() const { return position; }
	virtual ~Comparison() {}
	virtual std::string getOperator() const = 0;
private:
	Expression expr1;
	Expression expr2;
	Position position;
};

class LessThan : public Comparison {
public:
	LessThan(const Position& position, Expression& expr1, Expression& expr2) :
		Comparison(position, expr1, expr2) {};
	virtual ~LessThan() {}
	virtual std::string getOperator() const { return " < "; }
};

class LessOrEqual : public Comparison {
public:
	LessOrEqual(const Position& position, Expression& expr1, Expression& expr2) :
		Comparison(position, expr1, expr2) {};
	virtual ~LessOrEqual() {}
	virtual std::string getOperator() const { return " <= "; }
};

class GreaterThan : public Comparison {
public:
	GreaterThan(const Position& position, Expression& expr1, Expression& expr2) :
		Comparison(position, expr1, expr2) {};
	virtual ~GreaterThan() {}
	virtual std::string getOperator() const { return " > "; }
};

class GreaterOrEqual : public Comparison {
public:
	GreaterOrEqual(const Position& position, Expression& expr1, Expression& expr2) :
		Comparison(position, expr1, expr2) {};
	virtual ~GreaterOrEqual() {}
	virtual std::string getOperator() const { return " >= "; }
};

class Equal : public Comparison {
public:
	Equal(const Position& position, Expression& expr1, Expression& expr2) :
		Comparison(position, expr1, expr2) {};
	virtual ~Equal() {}
	virtual std::string getOperator() const { return " == "; }
};

class NotEqual : public Comparison {
public:
	NotEqual(const Position& position, Expression& expr1, Expression& expr2) :
		Comparison(position, expr1, expr2) {};
	virtual ~NotEqual() {}
	virtual std::string getOperator() const { return " != "; }
};

class ComparisonFactory {
	static std::map<TokenType, std::function<ComparisonPtr(const Position& position, Expression& expr1, Expression& expr2)>> generator;
	static std::array<TokenType, 6> tokens;
public:
	static bool isComparisonOperator(const TokenType& oper);
	static std::unique_ptr<Comparison> getComparison(const Position& position, Expression& expr1, Expression& expr2, const TokenType& oper);
};