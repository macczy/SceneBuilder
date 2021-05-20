#include "BasicObject.h"

std::map<TokenType, std::function<BasicObjectPtr(const Position& pos, Properties& properties)>> BasicObjectFactory::generator {
	{TokenType::RECTANGLE_KEYWORD, [](const Position& pos, Properties& properties) {
		return std::make_unique<Rectangle>(pos, properties);
	}},
	{TokenType::POLYGON_KEYWORD, [](const Position& pos, Properties& properties) {
		return std::make_unique<Polygon>(pos, properties);
	}},
	{TokenType::LINE_KEYWORD, [](const Position& pos, Properties& properties) {
		return std::make_unique<Line>(pos, properties);
	}},
	{TokenType::CIRCLE_KEYWORD, [](const Position& pos, Properties& properties) {
		return std::make_unique<Circle>(pos, properties);
	}},
};

std::array<TokenType, 4> BasicObjectFactory::basicObjectsNames = {
		TokenType::RECTANGLE_KEYWORD,
		TokenType::POLYGON_KEYWORD,
		TokenType::LINE_KEYWORD,
		TokenType::CIRCLE_KEYWORD
};

BasicObjectPtr BasicObjectFactory::getBasicObject(const Position& pos, Properties& properties, const Token& nameToken) {
	if (!isBasicObjectNameToken(nameToken)) return nullptr;
	auto lambda = generator.find(nameToken.getType());
	if (lambda == generator.end()) return nullptr;
	return lambda->second(pos, properties);
}

bool BasicObjectFactory::isBasicObjectNameToken(const Token& nameToken) {
	auto res = std::find(basicObjectsNames.begin(), basicObjectsNames.end(), nameToken.getType());
	return res != basicObjectsNames.end();
}



std::map<std::string, ReturnType> Rectangle::buildInProperties() {
	return {};
}


std::map<std::string, ReturnType> Circle::buildInProperties() {
	return {};
}


std::map<std::string, ReturnType> Line::buildInProperties() {
	return {};
}


std::map<std::string, ReturnType> Polygon::buildInProperties() {
	return {};
}