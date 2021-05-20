#pragma once
#include <tuple>
#include <string>

#include "../Util/Token.h"

using tripleHexValues = std::tuple<std::string, std::string, std::string>;

class Color {
public:
	Color(const Position& position, const std::string& hex1, const std::string& hex2, const std::string& hex3) : values(hex1, hex2, hex3), position(position) {};
	const tripleHexValues& getValues() { return values; }
	const Position& getPosition() const { return position; }
private:
	tripleHexValues values;
	Position position;
};

