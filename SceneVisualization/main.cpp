#include "CommonHeader.h"
#include "shader.h"
#include "ComplexObject.hpp"
#include "BasicObjectFactory.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <signal.h>
#include <fcntl.h>
#include <thread>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
constexpr unsigned int SCR_WIDTH = 800;
constexpr unsigned int SCR_HEIGHT = 800;

using namespace std;
int main()
{
    if (glfwInit() != GL_TRUE)
    {
        cout << "GLFW initialization failed" << endl;
        return -1;
    }
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Scene", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (glewInit() != GLEW_OK) {
        throw runtime_error("GLEW Initialization failed");
    }

    ShaderProgram shader("shaders/shader.vert", "shaders/shader.frag");
    const int shaderId = shader.get_programID();
    std::vector<ObjectPtr> objects;

    {//init objects
        objects.push_back(BasicObjectFactory::getCircle(.0f, .0f, 0.5f, { 1.0f, 1.0f, 0.0 }));
    }

    float deltaTime = 0.0f;	// Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    shader.use();
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        for (auto& object : objects) {
            object->draw(shaderId);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}