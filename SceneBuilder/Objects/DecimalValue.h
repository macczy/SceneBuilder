#pragma once
#include <string>
#include "../Util/Token.h"

class DecimalValue {
public:
	DecimalValue(const Position& position, const std::string& value) : value(value), position(position) {};
	const std::string& getValue() const { return value; }
	const Position& getPosition() const { return position; }
private:
	std::string value;
	Position position;
};

