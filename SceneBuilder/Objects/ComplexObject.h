#pragma once
#include <vector>
#include <map>
#include <variant>
#include "BasicObject.h"

class ComplexObject;
using ComplexObjectPtr = std::unique_ptr<ComplexObject>;

using ObjectPtr = std::unique_ptr<Object>;
class ObjectIdentifierPair;

using ObjectIdentifierPairPtr = std::unique_ptr<ObjectIdentifierPair>;
using Objects = std::vector<ObjectIdentifierPairPtr>;

class ComplexObject : public Object {
public:
	ComplexObject(const std::string& typeIdentifier, const Position& pos, Properties& properties)
		: Object(pos, properties), typeIdentifier(typeIdentifier) {}
	virtual ~ComplexObject() {}
	const std::string& getTypeIdentifier() { return typeIdentifier; }
protected:
	const std::string typeIdentifier;
	//Animations animations;
};


class ObjectIdentifierPair {
public:
	ObjectIdentifierPair(Identifier& ident, ObjectPtr& obj) : name(ident.getValue()), pos(ident.getPosition()), object(std::move(obj)) {}
	ObjectIdentifierPair(Identifier& ident, ComplexObjectPtr& obj) : name(ident.getValue()), pos(ident.getPosition()), object(std::move(obj)) {}
	ObjectIdentifierPair(Identifier& ident, BasicObjectPtr& obj) : name(ident.getValue()), pos(ident.getPosition()), object(std::move(obj)) {}
	std::string name;
	Position pos;
	ObjectPtr object;
	~ObjectIdentifierPair() {}
};

class ComplexObjectDeclaration {
public:
	ComplexObjectDeclaration(const std::string& name, const Position& pos, Properties& properties, Objects& objects)
		: name(name), pos(pos), properties(std::move(properties)), objects(std::move(objects)) {};
	Position& getPosition() { return pos; }
	Properties& getProperties() { return properties; }
	Objects& getObjects() { return objects; }
	const std::string& getName() const { return name; }
	virtual ~ComplexObjectDeclaration() {}
private:
	std::string name;
	Position pos; 
	Properties properties;
	Objects objects;
};
using ComplexObjectDeclarationPtr = std::unique_ptr<ComplexObjectDeclaration>;