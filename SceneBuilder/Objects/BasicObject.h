#pragma once
#include <vector>
#include <utility>
#include <memory>
#include <array>
#include <map>
#include <functional>
#include "../Util/ReturnType.h"
#include "Expression.h"
#include "Property.h"

class BasicObject;
using BasicObjectPtr = std::unique_ptr<BasicObject>;

class Object {
public:
	Object(const Position& pos, Properties& properties) : pos(pos), properties(std::move(properties)) {};
	Position& getPosition() { return pos; }
	Properties& getProperties() { return properties; }
	virtual std::string getObjectTypeName() const = 0;
	virtual ~Object() {}
	virtual std::map<std::string, ReturnType> buildInProperties() = 0;
protected:
	Position pos;
	Properties properties;
};

class BasicObject : public Object {
public:
	BasicObject(const Position& pos, Properties& properties) : Object(pos, properties) {};
	virtual ~BasicObject() {}
};

class Circle : public BasicObject {
public:
	Circle(const Position& pos, Properties& properties) : BasicObject(pos, properties) {};
	virtual std::map<std::string, ReturnType> buildInProperties();
	virtual ~Circle() {}

	virtual std::string getObjectTypeName() const { return "Circle"; };
};

class Polygon : public BasicObject {
public:
	Polygon(const Position& pos, Properties& properties) : BasicObject(pos, properties) {};
	virtual std::map<std::string, ReturnType> buildInProperties();
	virtual ~Polygon() {}
	virtual std::string getObjectTypeName() const { return "Polygon"; };
};

class Rectangle : public BasicObject {
public:
	Rectangle(const Position& pos, Properties& properties) : BasicObject(pos, properties) {};
	virtual std::map<std::string, ReturnType> buildInProperties();
	virtual ~Rectangle() {}
	virtual std::string getObjectTypeName() const { return "Rectangle"; };
};

class Line : public BasicObject {
public:
	Line(const Position& pos, Properties& properties) : BasicObject(pos, properties) {};
	virtual std::map<std::string, ReturnType> buildInProperties();
	virtual ~Line() {}
	virtual std::string getObjectTypeName() const { return "Line"; };
};

class BasicObjectFactory {
	static std::map<TokenType, std::function<BasicObjectPtr(const Position& pos, Properties& properties)>> generator;
	static std::array<TokenType, 4> basicObjectsNames;
public:
	static BasicObjectPtr getBasicObject(const Position& pos, Properties& properties, const Token& nameToken);
	static bool isBasicObjectNameToken(const Token& nameToken);
};