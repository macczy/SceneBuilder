#pragma once
#include "../Animations/AnimationSequence.h"
#include "../Objects/ComplexObject.h"
#include "Scene.h"
#include <map>
#include <string>

class SceneRoot
{
public:
	SceneRoot() {};
private:
	std::map<std::string, AnimationSequence> animations;
	std::map<std::string, ComplexObject> objects;
	Scene scene;
};

