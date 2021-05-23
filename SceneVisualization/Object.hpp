#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <memory>
#include <set>
#include <functional>
constexpr GLfloat M_PI = 3.14159265358979323846;

class Object {
public:
	virtual void animate() { 
		for (auto& animation : animations)
			animation();
	}
	virtual void draw(int shaderId) = 0;
	virtual void move(const glm::vec3& vector) = 0;
	virtual void move2(const glm::vec3& vector) = 0;
	virtual void scale(const glm::vec3& vector) = 0;
	virtual void scale2(const glm::vec3& vector) = 0;
	virtual void rotate(const glm::vec3& vector) = 0;
	virtual void rotate2(const glm::vec3& vector) = 0;

	virtual ~Object() {}
protected:
	std::set<std::function<void(void)>> animations;
};
using ObjectPtr = std::unique_ptr<Object>;