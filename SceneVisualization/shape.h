#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <iostream>
#include <vector>
#include <random>
#include <ctime>

constexpr GLfloat M_PI = 3.14159265358979323846;

class Shape {
public:
    Shape(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices, glm::vec2 direction = glm::vec2(0.005f, 0.001f)) :
        vertices(vertices), indices(indices), direction(direction) {
        GLfloat min = 0;
        GLfloat max = 2 * M_PI;
        GLfloat amp = 0.01f;
        GLfloat direcPhase = min + (double)(rand()) / ((double)(RAND_MAX / (max - min)));
        this->direction = glm::vec2(sin(direcPhase) * amp, cos(direcPhase) * amp);
        bot = top = vertices[1];
        right = left = vertices[0];
        for (int i = 5; i < vertices.size(); i += 5) {
            if (vertices[i + 1] < bot)
                bot = vertices[i + 1];
            if (vertices[i + 1] > top)
                top = vertices[i + 1];
            if (vertices[i] < left)
                left = vertices[i];
            if (vertices[i] > right)
                right = vertices[i];
        }
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);
        // Position
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        // Color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    void draw(int shaderId) {
        glm::mat4 modelTemp = model * scaling;
        glUniformMatrix4fv(glGetUniformLocation(shaderId, "model"), 1, GL_FALSE, &modelTemp[0][0]);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    void move() {
        model = glm::translate(glm::mat4(1.0f), glm::vec3(direction * speed, 0.0f)) * model;
        GLfloat verticalOffset = model[3][1];
        GLfloat horizontalOffset = model[3][0];
        if (direction[1] < 0 && verticalOffset + bot <= -1.0f || direction[1] > 0 && verticalOffset + top >= 1.0f) {
            direction[1] *= -1;
        }
        if (direction[0] < 0 && horizontalOffset + left <= -1.0f || direction[0] > 0 && horizontalOffset + right >= 1.0f) {
            direction[0] *= -1;
        }
    }

    void setDirection(glm::vec2 direction) {
        this->direction = direction;
    }

    void setSpeed(const GLfloat& speed) {
        this->speed = fmax(speed, 0.01f);
    }

    void incrementSpeed(int times = 1) {
        speed = fmin(speed + times * speedDelta, 10.0f);
    }

    void decrementSpeed(int times = 1) {
        speed = fmax(speed - times * speedDelta, 0.01f);
    }


protected:
    GLuint VAO, VBO, EBO;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 scaling = glm::mat4(1.0f);
    glm::vec2 direction;
    GLfloat bot, right, left, top;
    GLfloat speed = 0.01f;
    const GLfloat speedDelta = 0.01f;
};
