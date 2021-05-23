#include "shader.h"
using namespace std;

ShaderProgram::ShaderProgram(const GLchar* vertexPath, const GLchar* fragmentPath)
{
    // prepare vertex and fragment shaders
    string vertex_code = read_shader_code(vertexPath);
    GLuint vertex_id = compile_shader(vertex_code.c_str(), GL_VERTEX_SHADER);

    string fragment_code = read_shader_code(fragmentPath);
    GLuint fragment_id = compile_shader(fragment_code.c_str(), GL_FRAGMENT_SHADER);
    // link shader program

    program_id = glCreateProgram();
    glAttachShader(program_id, vertex_id);
    glAttachShader(program_id, fragment_id);
    glLinkProgram(program_id);

    // Print linking errors if any
    GLint success;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[512];
        glGetProgramInfoLog(program_id, sizeof(infoLog), NULL, infoLog);
        string msg = string("Shader program linking: ") + infoLog;
        throw runtime_error(msg.c_str());
    }
    // Delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex_id);
    glDeleteShader(fragment_id);
}

// Use the program
void ShaderProgram::use() const
{
    glUseProgram(get_programID());
}

// returns program ID
GLuint ShaderProgram::get_programID() const
{
    return program_id;
}

void ShaderProgram::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(program_id, name.c_str()), value);
}
void ShaderProgram::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(program_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void ShaderProgram::setTransformMatrix(const std::string& name, glm::mat4 matrix) {
    glUniformMatrix4fv(glGetUniformLocation(program_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}
void ShaderProgram::setFloat(const std::string& name, GLfloat value) const
{
    glUniform1f(glGetUniformLocation(program_id, name.c_str()), value);
}
void ShaderProgram::setVec3(const std::string& name, glm::vec3 value) const
{
    glUniform3f(glGetUniformLocation(program_id, name.c_str()), value[0], value[1], value[2]);
}


string read_shader_code(const GLchar* shaderPath)
{
    ifstream shader_file;
    shader_file.exceptions(ifstream::badbit);
    shader_file.open(shaderPath);
    stringstream shader_stream;
    shader_stream << shader_file.rdbuf();
    shader_file.close();
    return shader_stream.str();
}

GLuint compile_shader(const GLchar* shaderCode, GLenum shaderType)
{
    GLuint shader_id = glCreateShader(shaderType);
    glShaderSource(shader_id, 1, &shaderCode, NULL);
    glCompileShader(shader_id);

    // Print compile errors if any
    GLint success = 0;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader_id, sizeof(infoLog), NULL, infoLog);
        string msg = string("Shader compilation: ") + infoLog;
        throw runtime_error(msg.c_str());
    }
    return shader_id;
}