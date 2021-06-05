#include "Generator.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <set>
#include "../Exceptions/SyntaxError.h"
#include "../Exceptions/MissingRequiredProperty.h"

namespace fs = std::filesystem;

Generator::Generator(SceneRootPtr&& root) : root(std::move(root))
{

}

void Generator::run()
{
	fs::path targetDir = fs::current_path();
	targetDir /= "../SceneBuilderTargetDirectory";
	std::string command = "\"D:\\apps\\visual studio\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x64 && cd "
		+ targetDir.string() + " && cl /std:c++latest /MD /EHsc *.cpp /I..\\Include /link -LIBPATH:..\\Lib opengl32.lib glfw3.lib "
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

std::string Generator::getMembers(Properties& props) {
	std::stringstream returnStream;
	for (auto& prop : props) {
		if (prop->getName() == "animations") {
			auto& animationCalls = std::get<AnimationProperty>(prop->getValue()).getAnimationCalls();
			for (auto& animationCall : std::get<AnimationProperty>(prop->getValue()).getAnimationCalls()) {
				returnStream << "\t" << animationCall->getName() << " m_" << animationCall->getName() << ";\n";
			}
		}
	}
	return returnStream.str();
}

std::string Generator::getMembers(Objects& objects)
{
	std::stringstream returnStream;
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
	returnStream << "#include \"Animations.h\"\n";
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
	switch (value.getTimeSpecifier()) {
	case TimeDeclaration::TimeSpecifier::second:
		return value.getValue();
	case TimeDeclaration::TimeSpecifier::milisecond:
		return value.getValue() + "/1000";
	case TimeDeclaration::TimeSpecifier::minute:
		return value.getValue() + "*60";
	}
	return "0";
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
	returnStream << "\t MyScene() : Scene() {\n";
	for (auto& prop : scene->getProperties())
	{
		if (prop->getName() != "animations") {
			returnStream << "\t\t" << prop->getName() << " = " << generateExpression(prop->getValue()) << ";\n";
		}
	}

	for (auto& object : scene->getObjects())
	{
		//init object
		returnStream << generateSubObjectInitialization(object) << "\n";
	}
	returnStream << "\t}\n";
	//end constructor

	returnStream << generateAnimateSelf(scene->getProperties()) << '\n';

	returnStream << "public:\n" << getGetters(scene->getObjects());;
	returnStream << "private:\n" << 
		getMembers(scene->getObjects()) << '\n' 
		<< getMembers(scene->getProperties()) << '\n';
	returnStream << "};\n";
	return returnStream.str();
}


std::string Generator::generateAnimateSelf(Properties& properties)
{
	auto animationsIterator = std::find_if(properties.begin(), properties.end(), [](const PropertyPtr& prop) {
		return prop->getName() == "animations";
	});
	if (animationsIterator == properties.end()) {
		return "\tvoid animateSelf(float deltaTime) {\n\t}\n";
	}
	else {
		std::stringstream returnStream;

		returnStream << "\tvoid animateSelf(float deltaTime) {\n";

		auto& animationCalls = std::get<AnimationProperty>((*animationsIterator)->getValue()).getAnimationCalls();
		for (auto& animationCall : animationCalls) {
			returnStream << "\t\tm_" << animationCall->getName() << ".animate(deltaTime\n";
			for (auto& arg : animationCall->getArguments()) {
				//only use first argument for now, until I have an idea how to handle multiple
				returnStream << "\t\t\t, " << std::visit(ExpressionGeneratorVisitor(), arg[0]) << "\n"; //  arg[0];
			}
			returnStream << "\t\t);\n";
		}

		returnStream << "\t}\n";
		return returnStream.str();
	}
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
		if (prop->getName() != "animations") {
			//init property
			returnStream << "\t\t" << prop->getName() << " = " << generateExpression(prop->getValue()) << ";\n";
		}
	}

	for (auto& object : objectDeclaration->getObjects())
	{
		//init object
		returnStream << generateSubObjectInitialization(object) << "\n";
	}
	returnStream << "\t}\n";
	//end constructor

	returnStream << generateAnimateSelf(objectDeclaration->getProperties()) << '\n';

	returnStream << "public:\n" << getGetters(objectDeclaration->getObjects());;
	returnStream << "private:\n" <<
		getMembers(objectDeclaration->getObjects()) << '\n'
		<< getMembers(objectDeclaration->getProperties()) << '\n';
	returnStream << "};\n";
	return returnStream.str();
}

std::string Generator::generateWaitAnimation(Wait* animation, const std::string& time, const std::string animationArgs, const std::string& ident)
{
	std::stringstream returnStream;
	returnStream << " [deltaTime, this" << animationArgs << "]() {\t\t//Wait\n";
	returnStream << ident << "\tfloat timeToUse = deltaTime;\n";
	returnStream << ident << "\tif(deltaTime + totalTime > " << time << ") {\n";
	returnStream << ident << "\t\tfloat restTime = totalTime + deltaTime - " << time << ";\n";
	returnStream << ident << "\t\ttimeToUse = deltaTime - restTime;\n";
	returnStream << ident << "\t}\n";
	returnStream << ident << "\tif(timeToUse <= 0 ) {\n";
	returnStream << ident << "\t\treturn 0.0f;\n";
	returnStream << ident << "\t}\n";
	returnStream << ident << "\treturn timeToUse;\n";
	returnStream << ident << "}();\n";
	return returnStream.str();
}

std::string Generator::generateBasicAnimation(Animation* animation, const std::string& time, const std::string animationArgs, const std::string& ident)
{
	std::stringstream returnStream;
	
	auto& properties = animation->getProperties();

	auto objProperty = getPropertyByName(properties, "object");
	if (!objProperty)
		throw MissingRequiredProperty("object", "Animation", animation->getPosition());

	auto animatedProperty = getPropertyByName(properties, "property");
	if (!animatedProperty)
		throw MissingRequiredProperty("property", "Animation", animation->getPosition());

	auto changeByProperty = getPropertyByName(properties, "changeBy");
	if (!changeByProperty)
		throw MissingRequiredProperty("changeBy", "Animation", animation->getPosition());

	//check type here, once I want to distinguish property animation from other types
	//auto typeProperty = getPropertyByName(properties, "type");
	//if (!objProperty)
	//	throw MissingRequiredProperty("type", "Animation", animation->getPosition());

	std::string propertyName = std::visit(ExpressionGeneratorVisitor(), animatedProperty->getValue());
	std::string objectName = std::visit(ExpressionGeneratorVisitor(), objProperty->getValue());
	std::string valueString = std::visit(ExpressionGeneratorVisitor(), changeByProperty->getValue());

	returnStream << " [deltaTime, this" << animationArgs << "]() {\t\t//BasicAnimation\n";
	returnStream << ident << "\tfloat timeToUse = deltaTime;\n";
	returnStream << ident << "\tif(deltaTime + totalTime > " << time << ") {\n";
	returnStream << ident << "\t\tfloat restTime = totalTime + deltaTime - " << time << ";\n";
	returnStream << ident << "\t\ttimeToUse = deltaTime - restTime;\n";
	returnStream << ident << "\t}\n";
	returnStream << ident << "\tif(timeToUse <= 0 ) {\n";
	returnStream << ident << "\t\treturn 0.0f;\n";
	returnStream << ident << "\t}\n";
	returnStream << ident << "\t(" << objectName << ")->set" << propertyName << "((timeToUse/"<< time <<")*(\n";
	returnStream << ident << "\t\t" << valueString << ") + (" << objectName << ")->get" << propertyName << "());\n";
	returnStream << ident << "\treturn timeToUse;\n";
	returnStream << ident << "}();\n";

	return returnStream.str();
}
std::string Generator::generateParalelAnimation(ParalelAnimation* animation, const std::string& time, const std::string animationArgs, const std::string& ident)
{
	std::stringstream returnStream;

	auto& properties = animation->getProperties();

	returnStream << " [deltaTime, this" << animationArgs << "]() {\t\t//PralelAnimation\n";


	returnStream << ident << "\tfloat timeToUse = deltaTime;\n";
	returnStream << ident << "\tif(deltaTime + totalTime > " << time << ") {\n";
	returnStream << ident << "\t\tfloat restTime = totalTime + deltaTime - " << time << ";\n";
	returnStream << ident << "\t\ttimeToUse = deltaTime - restTime;\n";
	returnStream << ident << "\t}\n";
	returnStream << ident << "\tif(timeToUse <= 0 ) {\n";
	returnStream << ident << "\t\treturn 0.0f;\n";
	returnStream << ident << "\t}\n";


	auto& subAnimations = animation->getAnimations();
	for (int i = 0; i < subAnimations.size(); ++i) {
		auto& subAnimation = subAnimations[i];
		returnStream << ident << "\tfloat restTime" << i << " = " << generateSubAnimation(subAnimation, animationArgs, ident + "\t") << "\n";
	}

	returnStream << ident << "float restTime = std::max({ 0.0f, timeToUse";
	for (int i = 0; i < subAnimations.size(); ++i) {
		returnStream << ", restTime" << i;
	}
	returnStream << "});\n";

	returnStream << ident << "return restTime;\n";
	returnStream << ident << "}();\n";
	return returnStream.str();
}
std::string Generator::generateAnimationSequence(AnimationSequence* animation, const std::string& time, const std::string animationArgs, const std::string& ident)
{
	return ident + "not implemented yet";

}
std::string Generator::generateConditionalAnimation(ConditionalAnimation* animation, const std::string& time, const std::string animationArgs, const std::string& ident)
{
	return ident + "not implemented yet";
}

std::string Generator::generateSubAnimation(AnimationPtr& animation, const std::string& animationArgs, const std::string& ident) {
	std::stringstream returnStream;

	auto timeDeclarationIterator = std::find_if(animation->getProperties().begin(), animation->getProperties().end(), [](const PropertyPtr& prop) {return prop->getName() == "duration"; });

	if (timeDeclarationIterator == animation->getProperties().end())
		throw MissingRequiredProperty("duration", "Animation", animation->getPosition());
	
	std::string	time = std::visit(ExpressionGeneratorVisitor(), (*timeDeclarationIterator)->getValue());

	if (auto wait = dynamic_cast<Wait*>(animation.get()))
	{
		returnStream << generateWaitAnimation(wait, time, animationArgs, ident);
	}
	else if (auto paralelAnimation = dynamic_cast<ParalelAnimation*>(animation.get()))
	{
		returnStream << generateParalelAnimation(paralelAnimation, time, animationArgs, ident);
	}
	else if (auto animationSequence = dynamic_cast<AnimationSequence*>(animation.get()))
	{
		returnStream << generateAnimationSequence(animationSequence, time, animationArgs, ident);
	}
	else if (auto conditionalAnimation = dynamic_cast<ConditionalAnimation*>(animation.get()))
	{
		returnStream << generateConditionalAnimation(conditionalAnimation, time, animationArgs, ident);
	}
	else
	{
		returnStream << generateBasicAnimation(animation.get(), time, animationArgs, ident);
	}
	return returnStream.str();
}


std::string Generator::generateAnimations(std::vector<AnimationDeclarationPtr>& animations) {
	std::stringstream returnStream;
	returnStream << "#pragma once\n";
	returnStream << "#include \"Animation.h\"\n\n";
	returnStream << "#include <algorithm>\n\n";

	for (auto& animation : animations) {
		returnStream << "class " << animation->getName() << " : public Animation {\npublic:\n";
		returnStream << "\t" << animation->getName() << "() : Animation() {}\n";

		returnStream << "\tvoid animate(float deltaTime";


		std::string animationArgs;
		for (auto& arg : animation->getArgs()) {
			animationArgs += ", " + arg.getValue();
		}

		for (auto& arg : animation->getArgs()) {
			returnStream << ", auto " << arg.getValue();
		}
		returnStream << ") {\n";
		returnStream << "\t\tif(deltaTime <= 0) return;\n";
		returnStream << "\t\tif(state > " << animation->getAnimations().size() << ")\n";
		returnStream << "\t\t\t state = 0;\n";

		returnStream << "\t\tfloat usedTime = 0.0f;\n";

		returnStream << "\t\tswitch(state) {\n";

		auto& subAnimations = animation->getAnimations();
		for (int i = 0; i < subAnimations.size(); ++i) {
			auto& subAnimation = subAnimations[i];
			returnStream << "\t\tcase " << i << ": { \n";
			returnStream << "\t\tusedTime = " << generateSubAnimation(subAnimation, animationArgs, "\t\t\t") << "\t\tbreak;\n\t\t}\n";
		}
		returnStream << "\t\tdefault: state = 0; return;\n";
		returnStream << "\t\t}\n";

		returnStream << "\t\tif(usedTime < deltaTime) {\n";
		returnStream << "\t\t\ttotalTime = 0;\n";
		returnStream << "\t\t\t++state;\n";
		returnStream << "\t\t\tanimate(usedTime" << animationArgs << "); \n";
		returnStream << "\t\t}\n";
		returnStream << "\t\telse {\n";
		returnStream << "\t\t\ttotalTime += usedTime;\n";
		returnStream << "\t\t}\n";
		returnStream << "\t}\n";
		returnStream << "};\n\n";
	}
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

	{
		std::string animations = generateAnimations(root->getKnownAnimations());
		std::fstream outputAnimationsFile;
		outputAnimationsFile.open(targetDir / "Animations.h", std::fstream::out);
		outputAnimationsFile << animations;
		outputAnimationsFile.close();
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