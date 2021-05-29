#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include "Object.h"
#include <SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
using namespace std;

class BasicObject : public Object
{
protected:
	bool bVerticesChanged;
	glm::mat4 model;
	glm::mat4 scaling;
	glm::vec3 color;
	GLuint VAO, VBO, EBO;
	GLenum DRAW_MODE = GL_TRIANGLES;
	vector<GLfloat> vertices;
	vector<GLuint> indices;
public:
	glm::vec3 getColor() {
		return color;
	}

	BasicObject(const glm::vec3& color) :
		model(glm::mat4(1.0f)),
		scaling(glm::mat4(1.0f)), 
		color(color) {
			bVerticesChanged = false;
	}

	virtual ~BasicObject()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	virtual void draw(int shaderId) override
	{
		glm::mat4 modelTemp = model * scaling;

		glUniformMatrix4fv(glGetUniformLocation(shaderId, "model"), 1, GL_FALSE, &modelTemp[0][0]);
		glUniform3fv(glGetUniformLocation(shaderId, "color"), 1, &color[0]);

		if(bVerticesChanged)
			reinitVertices();

		glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void move(const glm::vec3& vector) override
	{
		model = translate(model, vector);
	}

	void move2(const glm::vec3& vector) override
	{
		model = translate(glm::mat4(1.0f), vector) * model;
	}

	void scale(const glm::vec3& vector) override
	{
		scaling = glm::scale(scaling, vector);
	}
	void scale2(const glm::vec3& vector) override
	{
		model = glm::scale(glm::mat4(1.0f), vector) * model;
	}

	void rotate(const glm::vec3& vector) override
	{
		model = glm::rotate(model, glm::radians(vector.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(vector.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(vector.z), glm::vec3(0.0f, 0.0f, 1.0f));
	}

	void rotate2(const glm::vec3& vector) override
	{
		model = glm::rotate(glm::mat4(1.0f), glm::radians(vector.x), glm::vec3(1.0f, 0.0f, 0.0f)) * model;
		model = glm::rotate(glm::mat4(1.0f), glm::radians(vector.y), glm::vec3(0.0f, 1.0f, 0.0f)) * model;
		model = glm::rotate(glm::mat4(1.0f), glm::radians(vector.z), glm::vec3(0.0f, 0.0f, 1.0f)) * model;
	}

protected:
	void init() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);

		// Position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
	}

	static std::vector<GLuint> autoCalculateIndices(const std::vector<GLfloat>& vertices) {
		std::vector<GLuint> indices;
		GLuint next_start = 0;
		const auto numberOfVertices = vertices.size() / 3;
		std::vector<bool> cutVertices;
		cutVertices.resize(numberOfVertices, false);
		for(auto i = 0; i < numberOfVertices - 2; i++) { //find n-2 triangles and generate sides
			int j, k, m;
			for (j = next_start; cutVertices[j % numberOfVertices] == true; j++);
			for (k = j + 1; cutVertices[k % numberOfVertices] == true; k++);
			for (m = k + 1; cutVertices[m % numberOfVertices] == true; m++);
			cutVertices[k % numberOfVertices] = true;
			indices.push_back(j % numberOfVertices);
			indices.push_back(k % numberOfVertices);
			indices.push_back(m % numberOfVertices);

			next_start = m % numberOfVertices;
		}
		return indices;
	}
	void reinitVertices() {
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(vertices[0]), &vertices[0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
};

class Rectangle : public BasicObject {
public:
	Rectangle(glm::vec3 color = { 0,0,0 }, glm::vec3 leftTopCorner = { 0,0,0 }, GLfloat width = 0, GLfloat height = 0) : BasicObject(color), width(width), height(height) {
		recalculateVertices();
		move(leftTopCorner);
		indices = { 0,1,2, 0,2,3 };
		init();
	}
	GLfloat getWidth() { return width; }
	void setWidth(GLfloat newWidth) { width = newWidth; recalculateVertices(); }
	GLfloat getHeight() { return height; }
	void setHeight(GLfloat newHeight) { height = newHeight; recalculateVertices(); }
private:
	void recalculateVertices() {
		vertices = {
			0, 0, 0,
			0, 0 - height, 0,
			0 + width, 0 - height, 0,
			0 + width, 0, 0
		};
		bVerticesChanged = true;
	}
	GLfloat width, height;
};

class Line : public BasicObject {
public:
	Line(const std::vector<glm::vec3>& points, glm::vec3 color = { 0,0,0 }, GLfloat width = 0) : BasicObject(color) {
		assert(points.size() > 1);
		DRAW_MODE = GL_LINES;
		for (auto& point : points) {
			vertices.push_back(point[0]);
			vertices.push_back(point[1]);
			vertices.push_back(point[2]);
		}
		indices.clear();
		for (int i = 0; i < points.size()-1; ++i) {
			indices.push_back(i);
			indices.push_back(i + 1);
		}
		init();
	}
	virtual void draw(int shaderId) override
	{
		glm::mat4 modelTemp = model * scaling;

		glUniformMatrix4fv(glGetUniformLocation(shaderId, "model"), 1, GL_FALSE, &modelTemp[0][0]);
		glUniform3fv(glGetUniformLocation(shaderId, "color"), 1, &color[0]);

		if (bVerticesChanged)
			reinitVertices();

		glBindVertexArray(VAO);

		glDrawElements(GL_LINES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
};

class Circle : public BasicObject {
public:
	Circle(glm::vec3 color = { 0,0,0 }, glm::vec3 leftTopCorner = { 0,0,0 }, GLfloat radius = 0) : BasicObject(color), radius(radius) {
		recalculateVertices();
		move(leftTopCorner);
		indices = autoCalculateIndices(vertices);
		init();
	}
	void setRadius(GLfloat newRadius) { radius = newRadius; recalculateVertices(); }
	void recalculateVertices() {
		constexpr GLuint numberOfEdges = 40;
		constexpr GLfloat factor = 2 * M_PI / numberOfEdges;
		vertices.clear();
		for (int j = 0; j < numberOfEdges; ++j) {
			GLfloat i = j * factor;
			vertices.push_back(sin(i) * radius);
			vertices.push_back(cos(i) * radius);
			vertices.push_back(0.f);
		}
		bVerticesChanged = true;
	}
private:
	GLfloat radius;
};

class Polygon : public BasicObject {
public:
	Polygon(const std::vector<glm::vec3>& points, glm::vec3 color = { 0,0,0 }) : BasicObject(color) {
		assert(points.size() >= 3);
		for (const auto& point : points) {
			vertices.push_back(point[0]);
			vertices.push_back(point[1]);
			vertices.push_back(point[2]);
		}
		indices = autoCalculateIndices(vertices);
		init();
	}
	const vector<GLfloat>& getVertices() { return vertices; }
};