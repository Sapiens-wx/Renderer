#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Transform.h"

class Mesh {
public:
	Mesh();
	void Gui();
public:
	Transform transform;
	std::vector<glm::vec3> vertices;
	std::vector<int> tris;
	std::string name;
};