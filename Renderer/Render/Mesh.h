#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <fstream>
#include "Transform.h"
#include "../Geometry.h"

class Shader;

class Vertex {
public:
	Vertex(const glm::vec3& pos) :pos(pos) {}
	Vertex(const glm::vec3& pos, const glm::vec2& uv) :pos(pos), uv(uv), normal(0,0,0) {}
	Vertex() {}
public:
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
};

class Mesh {
public:
	Mesh();
	void Gui();
	void LoadFromFile(const char* path);
	//update min/max of the bounds
	void UpdateBounds();
	inline Bounds GlobalBounds();
private:
	void LoadFromFile_Obj(std::ifstream& file);
public:
	Transform transform;
	std::vector<Vertex> vertices;
	std::vector<int> tris;
	std::string name;
	Shader* shader;
	Bounds bounds;
};

Bounds Mesh::GlobalBounds() {
	Bounds ret(bounds);
	ret.SetCenter(transform.position);
	return ret;
}