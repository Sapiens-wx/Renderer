#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <fstream>
#include "Transform.h"

class Shader;

class Vertex {
public:
	Vertex(const glm::vec3 pos) :pos(pos) {}
	Vertex() {}
public:
	glm::vec3 pos;
	glm::vec2 uv;
	glm::vec3 normal;
};

class Mesh {
public:
	Mesh();
	void Gui();
	void LoadFromFile(const char* path);
	void LoadFromFile_Obj(std::ifstream& file);
public:
	Transform transform;
	std::vector<Vertex> vertices;
	std::vector<int> tris;
	std::string name;
	Shader* shader;
};