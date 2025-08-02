#pragma once
#include <glm/glm.hpp>

class Transform {
public:
	glm::mat4 GetMatrix() const;
public:
	glm::vec3 position, rotation;
};