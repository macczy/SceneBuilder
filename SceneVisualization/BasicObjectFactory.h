#pragma once
#include "BasicObject.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

using BasicObjectPtr = std::unique_ptr<BasicObject>;

class BasicObjectFactory {
    BasicObjectFactory() = delete;
public:
    static BasicObjectPtr getPolygon(const std::vector<GLfloat>& vertices, const glm::vec3 color) {
        return autoCalculateIndicesAndReturnBasicObject(vertices, color);
    }

    static BasicObjectPtr getRectangle(GLfloat left, GLfloat top, GLfloat width, GLfloat height, const glm::vec3 color) {
        std::vector<GLfloat> vertices = {
            left, top, 0,
            left, top - height, 0, 
            left + width, top - height, 0,
            left + width, top, 0
        };
        return getRectangle(vertices, color);
    }

    //TODO
    static BasicObjectPtr getLine(std::pair<GLfloat, GLfloat> firstVertice, std::pair<GLfloat, GLfloat> secondVertice, GLfloat width, const glm::vec3 color) {
        std::vector<GLfloat> vertices = {
            firstVertice.first, firstVertice.second, 0,
            firstVertice.first, firstVertice.second + width, 0,
            secondVertice.first, secondVertice.second, 0,
            secondVertice.first, secondVertice.second + width, 0
        };
        return getRectangle(vertices, color);
    }
    static BasicObjectPtr getCircle(const GLfloat& x, const GLfloat& y, const GLfloat& radius, const glm::vec3 color) {
        constexpr GLuint numberOfEdges = 20;
        std::vector<GLfloat> vertices;
        for (int j = 0; j < numberOfEdges; ++j) { //generate half-cycle
            GLfloat i = j * 2 * M_PI / numberOfEdges;
            vertices.push_back(sin(i) * radius + x);
            vertices.push_back(cos(i) * radius + y);
            vertices.push_back(0);
        }
        return autoCalculateIndicesAndReturnBasicObject(vertices, color);
    }
private:

    static BasicObjectPtr getRectangle(const std::vector<GLfloat>& vertices, const glm::vec3 color) {
        std::vector<GLuint> indices = { 0,1,2, 0,2,3 };
        return std::make_unique<BasicObject>(vertices, indices, color);
    }

    static BasicObjectPtr autoCalculateIndicesAndReturnBasicObject(const std::vector<GLfloat>& vertices, const glm::vec3 color) {
        std::vector<GLuint> indices;
        GLuint next_start = 0;
        const auto numberOfVertices = vertices.size()/3;
        std::vector<bool> cutVertices;
        cutVertices.resize(numberOfVertices, false);
        for (auto i = 0; i < numberOfVertices - 2; i++) { //find n-2 triangles and generate oval-sides
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
        return std::make_unique<BasicObject>(vertices, indices, color);
    }
};