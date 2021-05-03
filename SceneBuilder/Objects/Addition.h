#pragma once
#include <variant>
#include "../Scanner/Token.h"
#include "Value.h"

class Addition {
public:
	enum class Operator {
		PLUS,
		MINUS
	};
	Addition(const Position& position, std::unique_ptr<Value> value1, std::unique_ptr<Value>  value2, const Operator& oper) :
		value1(std::move(value1)), value2(std::move(value2)), position(position), oper(oper) {};
	Value* getFirstValue() { return value1.get(); }
	Value* getSecondValue() { return value2.get(); }
	const Position& getPosition() const { return position; }
	const Operator& getOperator() const { return oper; }
private:
	std::unique_ptr<Value> value1;
	std::unique_ptr<Value> value2;
	Position position;
	Operator oper;
};