#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <memory>
#include <set>
#include <functional>
constexpr GLfloat M_PI = static_cast<GLfloat>(3.14159265358979323846);

class Object {
public:
	virtual void animate(float delta) {
		for (auto& animation : animations)
			animation(delta);
	}
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
		GLfloat deltaX = position.x - x;
		move(glm::vec3(deltaX, 0, 0));
		this->position.x = x;
	}

	void sety(GLfloat y)
	{
		GLfloat deltaY = position.y - y;
		move(glm::vec3(deltaY, 0, 0));
		this->position.y = y;
	}

	void setz(GLfloat z)
	{
		GLfloat deltaZ = position.z - z;
		move(glm::vec3(deltaZ, 0, 0));
		this->position.z = z;
	}

	void setposition(const glm::vec3& position)
	{
		move(this->position - position);
		this->position = position;
	}

	void setvertex(const glm::vec3& position)
	{
		move(this->position - position);
		this->position = position;
	}
	
protected:
	glm::vec3 position;
	std::set<std::function<void(float)>> animations;
};
using ObjectPtr = std::unique_ptr<Object>;