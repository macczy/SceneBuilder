#pragma once
#include <vector>
#include <utility>
#include <memory>
#include <array>
#include <map>
#include <functional>
#include "Expression.h"
#include "Property.h"

class BasicObject;
using BasicObjectPtr = std::unique_ptr<BasicObject>;

class BasicObject
{
public:
	BasicObject(const Position& pos, Properties& properties) : pos(pos), properties(std::move(properties)) {};
	Position& getPosition() { return pos; }
	Properties& getProperties() { return properties; }

	~BasicObject() {}
protected:
	Position pos;
	Properties properties;
};

class Circle : public BasicObject
{
public:
	Circle(const Position& pos, Properties& properties) : BasicObject(pos, properties) {};
	~Circle() {}
};

class Polygon : public BasicObject
{
public:
	Polygon(const Position& pos, Properties& properties) : BasicObject(pos, properties) {};
	~Polygon() {}
};

class Rectangle : public BasicObject
{
public:
	Rectangle(const Position& pos, Properties& properties) : BasicObject(pos, properties) {};
	~Rectangle() {}
};

class Line : public BasicObject
{
public:
	Line(const Position& pos, Properties& properties) : BasicObject(pos, properties) {};
	~Line() {}
};

class BasicObjectFactory {
	static std::map<std::string, std::function<BasicObjectPtr(const Position& pos, Properties& properties)>> generator;
	static std::array<std::string, 4> basicObjectsNames;
public:
	static BasicObjectPtr getBasicObject(const Position& pos, Properties& properties, const Token& nameToken);
	static bool isBasicObjectNameToken(const Token& nameToken);
};