#pragma once
#include "../Objects/ComplexObject.h"
#include "../Objects/AnimationDeclaration.h"
#include "Scene.h"
#include <map>
#include <string>

class SceneRoot {
public:
	SceneRoot(ScenePtr& scene, std::vector<ComplexObjectDeclarationPtr>& knownObjects,
		std::vector<AnimationDeclarationPtr>& knownAnimations) : scene(std::move(scene)), knownObjects(std::move(knownObjects)), knownAnimations(std::move(knownAnimations)) {};
private:
	ScenePtr scene;
	std::vector<ComplexObjectDeclarationPtr> knownObjects;
	std::vector<AnimationDeclarationPtr> knownAnimations;
};

