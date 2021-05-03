#pragma once
#include <string>
#include "../Scanner/Token.h"

class HexValue
{
public:
	HexValue(const Position& position, const std::string& value) : value(value), position(position) {};
	const std::string& getValue() { return value; }
	const Position& getPosition() const { return position; }
private:
	std::string value;
	Position position;
};
