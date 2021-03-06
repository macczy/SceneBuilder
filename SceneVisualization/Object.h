#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <memory>
#include <set>
#include <functional>
constexpr GLfloat M_PI = static_cast<GLfloat>(3.14159265358979323846);

class Object {
public:
	virtual void init() = 0;
	virtual void animate(float deltaTime) = 0;
	virtual void draw(int shaderId) = 0;
	virtual void move(const glm::vec3& vector) = 0;
	virtual void move2(const glm::vec3& vector) = 0;
	virtual void scale(const glm::vec3& vector) = 0;
	virtual void scale2(const glm::vec3& vector) = 0;
	virtual void rotate(const glm::vec3& vector) = 0;
	virtual void rotate2(const glm::vec3& vector) = 0;

	virtual ~Object() {}

	glm::vec3 getposition()
	{
		return position;
	}

	GLfloat getx()
	{
		return position.x;
	}
	GLfloat gety()
	{
		return position.y;
	}
	GLfloat getz()
	{
		return position.z;
	}

	void setx(GLfloat x)
	{
		GLfloat deltaX = x - position.x;
		move2(glm::vec3(deltaX, 0, 0));
		this->position.x = x;
	}

	void sety(GLfloat y)
	{
		GLfloat deltaY = y - position.y;
		move2(glm::vec3(0, deltaY, 0));
		this->position.y = -y;
	}

	void setz(GLfloat z)
	{
		GLfloat deltaZ = z - position.z;
		move2(glm::vec3(0, 0, deltaZ));
		this->position.z = z;
	}

	void setposition(const glm::vec3& position)
	{
		move2(position - this->position);
		this->position = position;
	}

	void setvertex(const glm::vec3& position)
	{
		move2(this->position - position);
		this->position = position;
	}
	
protected:
	glm::vec3 position;
};
using ObjectPtr = std::unique_ptr<Object>;