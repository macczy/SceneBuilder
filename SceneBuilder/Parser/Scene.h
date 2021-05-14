#pragma once
#include "../Objects/ComplexObject.h"

class Scene
{
public:
	Scene() {};
private:
	Objects objects;
};

using ScenePtr = std::unique_ptr<Scene>;