#include "BasicObject.h"

std::map<std::string, std::function<BasicObjectPtr(const Position& pos, Properties& properties)>> BasicObjectFactory::generator {
	{"Rectangle", [](const Position& pos, Properties& properties) {
		return std::make_unique<Rectangle>(pos, properties);
	}},
	{"Polygon", [](const Position& pos, Properties& properties) {
		return std::make_unique<Polygon>(pos, properties);
	}},
	{"Line", [](const Position& pos, Properties& properties) {
		return std::make_unique<Line>(pos, properties);
	}},
	{"Circle", [](const Position& pos, Properties& properties) {
		return std::make_unique<Circle>(pos, properties);
	}},
};

std::array<std::string, 4> BasicObjectFactory::basicObjectsNames = {
		"Rectangle",
		"Polygon",
		"Line",
		"Circle"
};

BasicObjectPtr BasicObjectFactory::getBasicObject(const Position& pos, Properties& properties, const Token& nameToken) {
	if (!isBasicObjectNameToken(nameToken)) return nullptr;
	auto lambda = generator.find(nameToken.getValue());
	if (lambda == generator.end()) return nullptr;
	return lambda->second(pos, properties);
}

bool BasicObjectFactory::isBasicObjectNameToken(const Token& nameToken) {
	if (nameToken.getType() != TokenType::TYPE_IDENTIFIER) return false;
	auto res = std::find(basicObjectsNames.begin(), basicObjectsNames.end(), nameToken.getValue());
	return res != basicObjectsNames.end();
}
