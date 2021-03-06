#pragma once
#include "CommonHeader.h"
#include <chrono>
#include <thread>

void processInput(GLFWwindow* window);

class Scene : public ComplexObject
{
public:
	Scene()
	{
	}

    virtual ~Scene() {}

	virtual void run()
	{
        if (glfwInit() != GL_TRUE)
        {
            throw std::runtime_error("GLFW initialization failed");
        }

        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_SAMPLES, 4);
        glEnable(GL_MULTISAMPLE);
        window = glfwCreateWindow((GLuint)width, (GLuint)height, "Scene", NULL, NULL);
        if (window == nullptr)
        {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(window);

        if (glewInit() != GLEW_OK) {
            throw runtime_error("GLEW Initialization failed");
        }
        glViewport(0, 0, (GLuint)width, (GLuint)height);

        glEnable(GL_DEPTH_TEST);

        shader = std::make_unique<ShaderProgram>("shaders/shader.vert", "shaders/shader.frag");
        const int shaderId = shader->get_programID();

        shader->use();
        {
            glm::mat4 view = glm::lookAt(
                glm::vec3(0.0f, 0.0f, 3.0f), //eye
                glm::vec3(0.0f, 0.0f, 0.0f), //center
                glm::vec3(0.0f, 1.0f, 0.0f)); //up
            shader->setMat4("view", view);
            glm::mat4 projection = glm::ortho(0.0f, width, 0.0f, height, 0.0f, 500.0f);
            shader->setMat4("projection", projection);
        }
        init();
        float deltaTime = 0.0f;	// Time between current frame and last frame
        float lastFrame = 0.0f;	// Time between current frame and last frame

        //can be used to lock framerate
        constexpr int LIMIT_FRAME_RATE_PER_SECOND = 120;
        constexpr float MAX_SINGLE_FRAME_TIME = 1.0 / LIMIT_FRAME_RATE_PER_SECOND;
        updatePosition();
        while (!glfwWindowShouldClose(window))
        {
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;

            //can be used to lock framerate
            //if (float difference = MAX_SINGLE_FRAME_TIME - deltaTime; difference > 0) {
            //    std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000 * difference)));
            //    lastFrame = lastFrame + MAX_SINGLE_FRAME_TIME;
            //    deltaTime = MAX_SINGLE_FRAME_TIME;
            //}
            //else {
                lastFrame = currentFrame;
            //}

            animate(deltaTime);
            draw(shaderId);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwTerminate();
	}
	virtual GLfloat getheight() { return height; }
	virtual GLfloat getwidth() { return width; }
    virtual void setheight(GLfloat height) { 
        this->height = height;
        updatePosition();
    }
    virtual void setwidth(GLfloat width) { 
        this->width = width;
        updatePosition();
    }

    virtual void updatePosition()
    {
        auto* monitor = glfwGetPrimaryMonitor();
        if (!monitor) return;
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        if (!mode) return;
        glfwSetWindowPos(window, mode->width / 2.0f - width / 2.0f, mode->height / 2.0f - height / 2.0f);
        glfwSetWindowSize(window, (GLuint)width, (GLuint)height);
        glViewport(0, 0, (GLuint)width, (GLuint)height);
        glm::mat4 projection = glm::ortho(0.0f, width, 0.0f, height, 0.0f, 500.0f);
        shader->setMat4("projection", projection);
    }
protected: 
    std::unique_ptr<ShaderProgram> shader;
    GLFWwindow* window = nullptr;
	GLfloat width = 800.0f;
	GLfloat height = 500.0f;
};


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}