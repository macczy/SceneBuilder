#include "SceneRoot.h"

Scene* SceneRoot::getScene()
{
	return scene.get();
}
std::vector<ComplexObjectDeclarationPtr>& SceneRoot::getKnownObjects()
{
	return knownObjects;
}
std::vector<AnimationDeclarationPtr>& SceneRoot::getKnownAnimations()
{
	return knownAnimations;
}