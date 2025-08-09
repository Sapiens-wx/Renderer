#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Color.h"

class Renderer;

struct GizmosVertex {
	GizmosVertex(const glm::vec3& position, const glm::vec3& col) :pos(position), color(col) {}
	glm::vec3 pos;
	glm::vec3 color;
};

class Gizmos {
	friend Renderer;
public:
	void AddLine(const glm::vec3& p1, const glm::vec3& p2, Color color=Color::White);
	//clears vertices
	inline void BeginFrame() { vertices.clear(); }
private:
	std::vector<GizmosVertex> vertices;
};
