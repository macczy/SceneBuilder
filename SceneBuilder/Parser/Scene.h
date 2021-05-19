#pragma once
#include "../Objects/ComplexObject.h"
#include "../Objects/Property.h"

class Scene {
public:
	Scene(const Position& pos, Objects& objects, Properties& properties) : pos(pos), objects(std::move(objects)), properties(std::move(properties)) {};
	Position& getPosition() { return pos; }
	Properties& getProperties() { return properties; }
	Objects& getObjects() { return objects; }
private:
	Position pos;
	Objects objects;
	Properties properties;
};

using ScenePtr = std::unique_ptr<Scene>;