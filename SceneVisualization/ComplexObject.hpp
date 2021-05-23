#pragma once
#include <vector>
#include <memory>
#include "Object.hpp"

class Complex : public Object {
public:
	void addChild(std::unique_ptr<Object>& child) {
		children.push_back(std::move(child));
	}
	void draw(int shaderId) override {
		for (auto& o : children) {
			o->draw(shaderId);
		}
	}

	void move(const glm::vec3& vector) override {
		for (auto& o : children) {
			o->move(vector);
		}
	}
	void move2(const glm::vec3& vector) override {
		for (auto& o : children) {
			o->move2(vector);
		}
	}
	void scale(const glm::vec3& vector) override {
		for (auto& o : children) {
			o->scale(vector);
		}
	}
	void scale2(const glm::vec3& vector) override {
		for (auto& o : children) {
			o->scale2(vector);
		}
	}
	void rotate(const glm::vec3& vector) override {
		for (auto& o : children) {
			o->rotate(vector);
		}
	}
	void rotate2(const glm::vec3& vector) override {
		for (auto& o : children) {
			o->rotate2(vector);
		}
	}
protected:
	std::vector<ObjectPtr> children;
};