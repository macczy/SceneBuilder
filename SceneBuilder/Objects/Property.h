#pragma once
#include "Expression.h"

class Property {
public:
	Property(const Position& pos, Identifier& name, Expression& expr) : pos(pos), name(name.getValue()), value(std::move(expr)) {}
	const Position& getPosition() const  { return pos; }
	const std::string& getName() const { return name; }
	const Expression& getValue() const { return value; }
	~Property() {}
private:
	Position pos;
	std::string name;
	Expression value;
};

using PropertyPtr = std::unique_ptr<Property>;

using Properties = std::vector<PropertyPtr>;