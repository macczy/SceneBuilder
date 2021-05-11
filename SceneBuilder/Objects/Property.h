#pragma once
#include "Expression.h"
class Property {
public:
	Property(const Position& pos, Identifier& name, Expression& expr) : pos(pos), name(std::move(name)), value(std::move(expr)) {}
	const Position& getPosition() const  { return pos; }
	const Identifier& getName() const { return name; }
	const Expression& getValue() const { return value; }
private:
	Position pos;
	Identifier name;
	Expression value;
};

using Properties = std::vector<Property>;