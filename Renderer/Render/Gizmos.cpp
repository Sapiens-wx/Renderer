#include "Gizmos.h"
#include "../Geometry.h"

void Gizmos::AddLine(const glm::vec3& p1, const glm::vec3& p2, Color color32) {
	glm::vec3 color = Color32ToFloat(color32);
	vertices.emplace_back(p1, color);
	vertices.emplace_back(p2, color);
}

void Gizmos::AddBounds(const Bounds& bounds, Color color32) {
	glm::vec3 color = Color32ToFloat(color32);
	const glm::vec3& min = bounds.min, max = bounds.max;
	vertices.emplace_back(min, color);
	vertices.emplace_back(min.x, min.y, max.z, color);
	vertices.emplace_back(min, color);
	vertices.emplace_back(min.x, max.y, min.z, color);
	vertices.emplace_back(min, color);
	vertices.emplace_back(max.x, min.y, min.z, color);
	vertices.emplace_back(max.x, min.y, min.z, color);
	vertices.emplace_back(max.x, max.y, min.z, color);
	vertices.emplace_back(max.x, min.y, min.z, color);
	vertices.emplace_back(max.x, min.y, max.z, color);
	vertices.emplace_back(min.x, max.y, min.z, color);
	vertices.emplace_back(max.x, max.y, min.z, color);
	vertices.emplace_back(min.x, max.y, min.z, color);
	vertices.emplace_back(min.x, max.y, max.z, color);
	vertices.emplace_back(min.x, min.y, max.z, color);
	vertices.emplace_back(max.x, min.y, max.z, color);
	vertices.emplace_back(min.x, min.y, max.z, color);
	vertices.emplace_back(min.x, max.y, max.z, color);
	vertices.emplace_back(max, color);
	vertices.emplace_back(max.x, max.y, min.z, color);
	vertices.emplace_back(max, color);
	vertices.emplace_back(max.x, min.y, max.z, color);
	vertices.emplace_back(max, color);
	vertices.emplace_back(min.x, max.y, max.z, color);
}