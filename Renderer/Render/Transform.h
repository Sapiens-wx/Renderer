#pragma once
#include <glm/glm.hpp>

class Transform {
public:
	glm::mat4 Local2World() const;
	glm::mat4 World2Local() const;
	void Gui();
public:
	glm::vec3 position, rotation;
};