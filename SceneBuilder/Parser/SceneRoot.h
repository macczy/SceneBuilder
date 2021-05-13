#pragma once
#include "../Objects/ComplexObject.h"
#include "../Objects/AnimationDeclaration.h"
#include "Scene.h"
#include <map>
#include <string>

class SceneRoot
{
public:
	SceneRoot() {};
private:
	//std::map<std::string, AnimationDeclaration> animations;
	//std::map<std::string, ComplexObject> objects;
	Scene scene;
};

