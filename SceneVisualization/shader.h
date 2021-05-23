#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "CommonHeader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>	



std::string read_shader_code(const GLchar* shaderPath);
GLuint compile_shader(const GLchar* shaderCode, GLenum shaderType);


class ShaderProgram
{
	GLuint program_id; 	// The program ID
public:
	// Constructor reads and builds the shader
	ShaderProgram(const GLchar* vertexPath, const GLchar* fragmentPath);

	// Use the program
	void use() const;

	// returns program ID
	GLuint get_programID() const;
	void setInt(const std::string& name, int value) const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;
	void setTransformMatrix(const std::string& name, glm::mat4 matrix);
	void setFloat(const std::string& name, GLfloat value) const;
	void setVec3(const std::string& name, glm::vec3 value) const;
};