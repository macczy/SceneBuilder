#include "Generator.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <set>

namespace fs = std::filesystem;

Generator::Generator(SceneRootPtr&& root) : root(std::move(root))
{

}

void Generator::run()
{
	fs::path targetDir = fs::current_path();
	targetDir /= "../SceneBuilderTargetDirectory";
	std::string command = "\"D:\\apps\\visual studio\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x64 && cd "
		+ targetDir.string() + " && cl /std:c++17 /MD /EHsc *.cpp /I..\\Include /link -LIBPATH:..\\Lib opengl32.lib glfw3.lib "
		"SOIL.lib glew64s.lib kernel32.lib user32.lib gdi32.lib winspool.lib "
		"comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NODEFAULTLIB:MSVCRTD /NODEFAULTLIB:LIBCMT /out:SceneVisualization.exe "
		" && SceneVisualization.exe";
	system(command.c_str());
}

std::string Generator::getGetters(Objects& objects)
{

	std::stringstream returnStream;

	auto basicProperites = { "width", "height", "x", "y", "z" };
	for (auto property : basicProperites)
	{
		returnStream << "\tGLfloat get" << property << "() { return " << property << ";}\n";
	}
	for (auto& obj : objects)
	{
		if (auto complex = dynamic_cast<ComplexObject*>(obj->object.get()); complex)
		{
			returnStream << "\t" << complex->getTypeIdentifier() << "* get" << obj->name << "() { return " << obj->name << ";}\n";
		}
		else if (auto basic = dynamic_cast<BasicObject*>(obj->object.get()); basic)
		{
			if (auto circle = dynamic_cast<Circle*>(basic); circle)
			{
				returnStream << "\tCircle* get" << obj->name << "() { return " << obj->name << ";}\n";
			}
			else if (auto circle = dynamic_cast<Rectangle*>(basic); circle)
			{
				returnStream << "\tRectangle* get" << obj->name << "() { return " << obj->name << ";}\n";
			}
			else if (auto circle = dynamic_cast<Polygon*>(basic); circle)
			{
				returnStream << "\tPolygon* get" << obj->name << "() { return " << obj->name << ";}\n";
			}
			else if (auto circle = dynamic_cast<Line*>(basic); circle)
			{
				returnStream << "\tLine* get" << obj->name << "() { return " << obj->name << ";}\n";
			}
		}
	}
	return returnStream.str();
}

std::string Generator::getMembers(Objects& objects)
{
	std::stringstream returnStream;
	auto basicProperites = { "width", "height", "x", "y", "z" };
	for (auto property : basicProperites)
	{
		returnStream << "\tGLfloat " << property << ";\n";
	}
	for (auto& obj : objects)
	{
		if (auto complex = dynamic_cast<ComplexObject*>(obj->object.get()); complex)
		{
			returnStream << "\t" << complex->getTypeIdentifier() << "* " << obj->name << ";\n";
		}
		else if (auto basic = dynamic_cast<BasicObject*>(obj->object.get()); basic)
		{
			if (auto circle = dynamic_cast<Circle*>(basic); circle)
			{
				returnStream << "\tCircle* " << obj->name << ";\n";
			}
			else if (auto circle = dynamic_cast<Rectangle*>(basic); circle)
			{
				returnStream << "\tRectangle* " << obj->name << ";\n";
			}
			else if (auto circle = dynamic_cast<Polygon*>(basic); circle)
			{
				returnStream << "\tPolygon* " << obj->name << ";\n";
			}
			else if (auto circle = dynamic_cast<Line*>(basic); circle)
			{
				returnStream << "\tLine* " << obj->name << ";\n";
			}
		}
	}
	return returnStream.str();
}


std::string Generator::getClassDeclaration(ComplexObjectDeclarationPtr& objectDeclaration)
{
	std::stringstream returnStream;
	returnStream << "include \"ComplexObject.h\"\n";
	returnStream << "include \"BasicObject.h\"\n";
	std::set<std::string> classesToInclude;
	for (auto& obj : objectDeclaration->getObjects())
	{
		if (auto complex = dynamic_cast<ComplexObject*>(obj->object.get()); complex)
		{
			classesToInclude.insert(complex->getTypeIdentifier());
		}
	}
	for (const auto& className : classesToInclude)
	{
		returnStream << "include \"" << className << ".h\"\n";
	}

	returnStream << "class " + objectDeclaration->getName() + " : public ComplexObject {\n";
	returnStream << "public: \n";

	//constructor
	returnStream << objectDeclaration->getName() << "() {";
	
	
	
	returnStream << "}\n";
	//end constructor

	std::string members = getMembers(objectDeclaration->getObjects());
	std::string getters = getGetters(objectDeclaration->getObjects());

	returnStream << "public:\n" << getters;
	returnStream << "private:\n" << members;
	returnStream << "}\n";
	return returnStream.str();
}

bool Generator::generate(const std::string& filename)
{
	fs::path targetDir = fs::current_path();
	targetDir /= "../SceneBuilderTargetDirectory";
	fs::remove_all(targetDir);
	fs::create_directory(targetDir);

	const fs::path sourceDirectories = fs::current_path() / "../SceneVisualization/";

	fs::copy(sourceDirectories, targetDir, fs::copy_options::recursive);

	for (auto& animationDeclaration : root->getKnownAnimations())
	{
		std::cout << "Would declare animation now: " << animationDeclaration->getName() << std::endl;
	}

	for (auto& objectDeclaration : root->getKnownObjects())
	{
		std::cout << "Would declare object: " << objectDeclaration->getName() << std::endl;
		std::string declaration = getClassDeclaration(objectDeclaration);
		std::cout << declaration << std::endl;
		std::fstream outputClassFile;
		outputClassFile.open(targetDir / (objectDeclaration->getName() + ".h"), std::fstream::out);
		outputClassFile << declaration;
		outputClassFile.close();
	}
	if(root->getScene())
		std::cout << "Would declare scene now " << std::endl;

	return true;
}