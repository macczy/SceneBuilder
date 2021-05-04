#pragma once
#include <memory>
#include "../Scanner/Token.h"
#include "Value.h"

class Comparison
{
public:
	Comparison(const Position& position, std::unique_ptr<Value> value1, std::unique_ptr<Value>  value2) :
		value1(std::move(value1)), value2(std::move(value2)), position(position) {};

	Value* getFirstValue() { return value1.get(); }
	Value* getSecondValue() { return value2.get(); }
	const Position& getPosition() { return position; }
	virtual ~Comparison() {}
private:
	std::unique_ptr<Value> value1;
	std::unique_ptr<Value> value2;
	Position position;
};

class LessThan : public Comparison
{
public:
	LessThan(const Position& position, std::unique_ptr<Value> value1, std::unique_ptr<Value>  value2) :
		Comparison(position, std::move(value1), std::move(value2)) {};
	virtual ~LessThan() {}
};

class LessOrEqual : public Comparison
{
public:
	LessOrEqual(const Position& position, std::unique_ptr<Value> value1, std::unique_ptr<Value>  value2) :
		Comparison(position, std::move(value1), std::move(value2)) {};
	virtual ~LessOrEqual() {}
};

class GreaterThan : public Comparison
{
public:
	GreaterThan(const Position& position, std::unique_ptr<Value> value1, std::unique_ptr<Value>  value2) :
		Comparison(position, std::move(value1), std::move(value2)) {};
	virtual ~GreaterThan() {}
};

class GreaterOrEqual : public Comparison
{
public:
	GreaterOrEqual(const Position& position, std::unique_ptr<Value> value1, std::unique_ptr<Value>  value2) :
		Comparison(position, std::move(value1), std::move(value2)) {};
	virtual ~GreaterOrEqual() {}
};

class Equal : public Comparison
{
public:
	Equal(const Position& position, std::unique_ptr<Value> value1, std::unique_ptr<Value>  value2) :
		Comparison(position, std::move(value1), std::move(value2)) {};
	virtual ~Equal() {}
};

class NotEqual : public Comparison
{
public:
	NotEqual(const Position& position, std::unique_ptr<Value> value1, std::unique_ptr<Value>  value2) :
		Comparison(position, std::move(value1), std::move(value2)) {};
	virtual ~NotEqual() {}
};

namespace ComparisonFactory
{
	bool isComparisonOperator(const Token::TokenType& oper);
	std::unique_ptr<Comparison> getComparison(const Position& position, std::unique_ptr<Value> value1, std::unique_ptr<Value> value2, const Token::TokenType& oper);
};