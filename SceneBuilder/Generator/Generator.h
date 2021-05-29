#pragma once
#include "../Objects/Expression.h"
#include "../Parser/SceneRoot.h"

class Generator
{
public:
	Generator(SceneRootPtr&& root);
	bool generate(const std::string& filename);
	void run();
	std::string getClassDeclaration(ComplexObjectDeclarationPtr& objectDeclaration);
	std::string getMembers(Objects& objects);
	std::string getGetters(Objects& objects);
private:
	SceneRootPtr root;
};

