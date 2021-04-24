#pragma once
#include <tuple>
#include <string>

#include "../Scanner/Token.h"

using triplePointValues = std::tuple<std::string, std::string, std::string>;

class Point
{
public:
	Point(const Position& position, const std::string& x = "0", const std::string& y = "0", const std::string& z = "0") : values(x, y, z), position(position) {};
	const triplePointValues& getValues() { return values; }
	const Position& getPosition() { return position; }
private:
	triplePointValues values;
	Position position;
};

