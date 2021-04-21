#pragma once
#include "AnimationSequence.h"
#include "ComplexObject.h"
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

