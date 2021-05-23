#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include "Object.hpp"
#include <SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
using namespace std;

class BasicObject : public Object
{
	glm::mat4 model;
	glm::mat4 scaling;
	glm::vec3 color;
protected:
	GLuint VAO, VBO, EBO;

	vector<GLfloat> vertices;
	vector<GLuint> indices;
public:

	glm::vec3 getColor() {
		return color;
	}

	BasicObject(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices, const glm::vec3& color) :
		model(glm::mat4(1.0f)),
		scaling(glm::mat4(1.0f)), 
		indices(indices), 
		vertices(vertices), 
		color(color) {
			init();
	}

	virtual ~BasicObject()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	void draw(int shaderId) override
	{
		glm::mat4 modelTemp = model * scaling;

		glUniformMatrix4fv(glGetUniformLocation(shaderId, "model"), 1, GL_FALSE, &modelTemp[0][0]);
		glUniform3fv(glGetUniformLocation(shaderId, "color"), 1, &color[0]);

		glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
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

private:
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
};