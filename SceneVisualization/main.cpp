#include "CommonHeader.h"
#include "shader.h"
#include "ComplexObject.h"
#include "BasicObject.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <signal.h>
#include <fcntl.h>
#include <thread>
#include <vector>

void processInput(GLFWwindow* window);

// settings
constexpr GLfloat SCR_WIDTH = 800.0f;
constexpr GLfloat SCR_HEIGHT = 500.0f;

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

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow((GLuint)SCR_WIDTH, (GLuint)SCR_HEIGHT, "Scene", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        throw runtime_error("GLEW Initialization failed");
    }
    glViewport(0, 0, (GLuint)SCR_WIDTH, (GLuint)SCR_HEIGHT);

    glEnable(GL_DEPTH_TEST);

    ShaderProgram shader("shaders/shader.vert", "shaders/shader.frag");
    const int shaderId = shader.get_programID();
    std::vector<ObjectPtr> objects;

    {//init objects
        objects.push_back(std::make_unique<Circle>(glm::vec3{ 1.0f, 1.0f, 0.0 }, glm::vec3{ .0f, .0f, 0.0f }, 50.5f));
        objects.push_back(std::make_unique<Rectangle>(glm::vec3{ 1.0f, 0.0f, 0.0 }, glm::vec3{ .0f, 1.0f, 0.5f }, .5f, 0.5f));

        std::vector<glm::vec3> points{ glm::vec3{ .0f, .0f, 0.4f }, glm::vec3{ -0.5f, 1.0f, 1.0f } };
        objects.push_back(std::make_unique<Line>(points, glm::vec3{ 0.0f, 0.0f, 1.0 }, 0.1f));

        std::vector<glm::vec3> points2{ 
            glm::vec3{ 0.1, 0.1f, 0.4f },glm::vec3{ 0.1f, 0.5f, 0.4f },
            glm::vec3{ 0.5f, 0.1f, 0.4f },glm::vec3{ 0.5, 0.5f, 0.4f },
            glm::vec3{ 0.7f, 1.0f, 0.4f }
        };
        objects.push_back(std::make_unique<Polygon>(points2, glm::vec3{ 0.5f, 0.5f, 0.5f }));
    }


    shader.use();
    {
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 direction = glm::normalize(cameraPos - cameraTarget);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraRight = glm::normalize(glm::cross(up, direction));
        glm::vec3 cameraUp = glm::cross(direction, cameraRight);
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::ortho(-SCR_WIDTH/2, SCR_WIDTH/2, -SCR_HEIGHT/2, SCR_HEIGHT/2, 0.0f, 500.0f);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
    }

    double deltaTime = 0.0f;	// Time between current frame and last frame
    double lastFrame = 0.0f; // Time of last frame

    while (!glfwWindowShouldClose(window))
    {
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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