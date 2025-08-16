#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Color.h"

class Renderer;
class Bounds;

struct GizmosVertex {
	GizmosVertex(const glm::vec3& position, const glm::vec3& col) :pos(position), color(col) {}
	GizmosVertex(float x, float y, float z, const glm::vec3& col) :pos(x,y,z), color(col) {}
	glm::vec3 pos;
	glm::vec3 color;
};

class Gizmos {
	friend Renderer;
public:
	static void AddLine(const glm::vec3& p1, const glm::vec3& p2, Color color=Color::White);
	static void AddBounds(const Bounds& bounds, Color color=Color::White);
	static Gizmos& Get();
	//clears vertices
	static inline void BeginFrame() { Gizmos::Get().vertices.clear(); }
private:
	std::vector<GizmosVertex> vertices;
};
