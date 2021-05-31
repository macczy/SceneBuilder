#include "Generator.h"
#include <fstream>
#include <iostream>
#include <sstream>
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

	//auto basicProperites = { "width", "height", "x", "y", "z" };
	//for (auto property : basicProperites)
	//{
	//	returnStream << "\tGLfloat get" << property << "() { return " << property << ";}\n";
	//}
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
	//auto basicProperites = { "width", "height", "x", "y", "z" };
	//for (auto property : basicProperites)
	//{
	//	returnStream << "\tGLfloat " << property << ";\n";
	//}
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

std::string Generator::getIncludeStatements(Objects& objects)
{
	std::stringstream returnStream;
	returnStream << "#include \"ComplexObject.h\"\n";
	returnStream << "#include \"BasicObject.h\"\n";
	for (auto& obj : objects)
	{
		if (auto complex = dynamic_cast<ComplexObject*>(obj->object.get()); complex)
		{
			returnStream << "#include \"" << complex->getTypeIdentifier() << ".h\"\n";
		}
	}
	return returnStream.str();
}

std::string Generator::LogicalExpressionGeneratorVisitor::operator()(const LogicalExpressionPtr& value)
{
	std::stringstream returnStream;
	returnStream << "(" << std::visit(LogicalExpressionGeneratorVisitor(), *value->getFirstValue()) << ")";
	returnStream << value->getOperator();
	returnStream << "(" << std::visit(LogicalExpressionGeneratorVisitor(), *value->getSecondValue()) << ")";
	return returnStream.str();
}

std::string Generator::LogicalExpressionGeneratorVisitor::operator()(const ComparisonPtr& value)
{
	std::stringstream returnStream;
	returnStream << "(" << std::visit(ExpressionGeneratorVisitor(), value->getFirstExpression()) << ")";
	returnStream << value->getOperator();
	returnStream << "(" << std::visit(ExpressionGeneratorVisitor(), value->getSecondExpression()) << ")";
	return returnStream.str();
}

std::string Generator::ExpressionGeneratorVisitor::operator()(const DecimalValue& value)
{
	return value.getValue();
}
std::string  Generator::ExpressionGeneratorVisitor::operator()(const Color& value)
{
	auto& [r, g, b] = value.getValues();
	return "glm::vec3(" + r + "," + g + "," + b + ")";
}
std::string  Generator::ExpressionGeneratorVisitor::operator()(const Point& value)
{
	auto& [x, y, z] = value.getValues();
	return "glm::vec3(" + x.getValue() + "," + y.getValue() + "," + z.getValue() + ")";
}
std::string  Generator::ExpressionGeneratorVisitor::operator()(const PointArray& value)
{
	auto& points = value.getArray();
	std::stringstream returnStream;
	returnStream << "std::vector<glm::vec3>({";
		
	for (auto& point : points)
	{
		auto& [x, y, z] = point.getValues();
		returnStream << "\n\t\t\tglm::vec3(" << x.getValue() << "," << y.getValue() << "," << z.getValue() << "),";
	}
	if(returnStream.str().back() == ',')
		returnStream.seekp(-1, std::ios_base::end);
	returnStream << "\n\t\t})";
	return returnStream.str();
}
std::string  Generator::ExpressionGeneratorVisitor::operator()(const TimeDeclaration& value)
{
	return "";
}
std::string  Generator::ExpressionGeneratorVisitor::operator()(const ConstantIdentifier& value)
{
	return "";
}
std::string  Generator::ExpressionGeneratorVisitor::operator()(const Identifier& value)
{
	std::stringstream returnStream;
	returnStream << value.getValue();
	auto nextValue = &value;
	while (nextValue->hasNext())
	{
		nextValue = nextValue->getNext();
		returnStream << "->get" << nextValue->getValue() << "()";
	}
	return returnStream.str();
}
std::string  Generator::ExpressionGeneratorVisitor::operator()(const AnimationProperty& value)
{
	return "";
}
std::string  Generator::ExpressionGeneratorVisitor::operator()(const TernaryExpressionPtr& value)
{
	std::stringstream returnStream;
	returnStream << "(" << std::visit(LogicalExpressionGeneratorVisitor(), *value->getCondition()) << ")";
	returnStream << "?";
	returnStream << "(" << std::visit(ExpressionGeneratorVisitor(), value->getTrueExpression()) << ")";
	returnStream << ":";
	returnStream << "(" << std::visit(ExpressionGeneratorVisitor(), value->getFalseExpression()) << ")";
	return returnStream.str();
}
std::string  Generator::ExpressionGeneratorVisitor::operator()(const MultiplicationPtr& value)
{
	std::stringstream returnStream;
	returnStream << "(" << std::visit(ExpressionGeneratorVisitor(), value->getFirstExpression()) << ")";
	returnStream << value->getOperator();
	returnStream << "(" << std::visit(ExpressionGeneratorVisitor(), value->getSecondExpression()) << ")";
	return returnStream.str();
}
std::string  Generator::ExpressionGeneratorVisitor::operator()(const AdditionPtr& value)
{
	std::stringstream returnStream;
	returnStream << "(" << std::visit(ExpressionGeneratorVisitor(), value->getFirstExpression()) << ")";
	returnStream << value->getOperator();
	returnStream << "(" << std::visit(ExpressionGeneratorVisitor(), value->getSecondExpression()) << ")";
	return returnStream.str();
}

std::string Generator::generateExpression(const Expression& expr)
{
	return "(" + std::visit(ExpressionGeneratorVisitor(), expr) + ")";
}

std::string Generator::generateSubObjectInitialization(ObjectIdentifierPairPtr& object)
{
	std::stringstream returnStream;
	returnStream << "\t\t" << object->name << " = ";
	returnStream << "new " << object->object->getObjectTypeName() << "();\n";
	for (auto& prop : object->object->getProperties()) {
		returnStream << "\t\t" << object->name << "->set" << prop->getName() << "(";
		returnStream << "(" << std::visit(ExpressionGeneratorVisitor(), prop->getValue()) << ")";
		returnStream << ");\n";
	}
	returnStream << "\t\tchildren.push_back(std::unique_ptr<" << object->object->getObjectTypeName() << ">(" << object->name << "));\n";
	return returnStream.str();
}


std::string Generator::generateSceneClass(Scene* scene)
{
	std::stringstream returnStream;

	returnStream << "#pragma once\n";
	returnStream << "#include \"Scene.h\"\n";

	returnStream << getIncludeStatements(scene->getObjects()) << "\n";

	returnStream << "class MyScene : public Scene {\n";
	returnStream << "public: \n";

	//constructor
	returnStream << "\t Scene() {\n";
	for (auto& prop : scene->getProperties())
	{
		if (prop->getName() == "animations") continue;
		//init property
		returnStream << "\t\t" << prop->getName() << " = " << generateExpression(prop->getValue()) << ";\n";

	}

	for (auto& object : scene->getObjects())
	{
		//init object
		returnStream << generateSubObjectInitialization(object) << "\n";
	}
	returnStream << "\t}\n";
	//end constructor

	std::string members = getMembers(scene->getObjects());
	std::string getters = getGetters(scene->getObjects());

	returnStream << "public:\n" << getters;
	returnStream << "private:\n" << members;
	returnStream << "};\n";
	return returnStream.str();
}

std::string Generator::getClassDeclaration(ComplexObjectDeclarationPtr& objectDeclaration)
{
	std::stringstream returnStream;

	returnStream << "#pragma once\n";

	returnStream << getIncludeStatements(objectDeclaration->getObjects()) << "\n";

	returnStream << "class " + objectDeclaration->getName() + " : public ComplexObject {\n";
	returnStream << "public: \n";

	//constructor
	returnStream << "\t" << objectDeclaration->getName() << "() {\n";
	for (auto& prop : objectDeclaration->getProperties())
	{
		if (prop->getName() == "animations") continue;
		//init property
		returnStream << "\t\t" << prop->getName() << " = " << generateExpression(prop->getValue()) << ";\n";

	}

	for (auto& object : objectDeclaration->getObjects())
	{
		//init object
		returnStream << generateSubObjectInitialization(object) << "\n";
	}
	returnStream << "\t}\n";
	//end constructor

	std::string members = getMembers(objectDeclaration->getObjects());
	std::string getters = getGetters(objectDeclaration->getObjects());

	returnStream << "public:\n" << getters;
	returnStream << "private:\n" << members;
	returnStream << "};\n";
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
		std::string declaration = getClassDeclaration(objectDeclaration);
		std::cout << declaration << std::endl;
		std::fstream outputClassFile;
		outputClassFile.open(targetDir / (objectDeclaration->getName() + ".h"), std::fstream::out);
		outputClassFile << declaration;
		outputClassFile.close();
	}
	if (root->getScene())
	{
		std::string scene = generateSceneClass(root->getScene());
		std::cout << scene << std::endl;
		std::fstream outputClassFile;
		outputClassFile.open(targetDir / "MyScene.h", std::fstream::out);
		outputClassFile << scene;
		outputClassFile.close();
	}
	return true;
}