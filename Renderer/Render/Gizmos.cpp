#include "Gizmos.h"
#include "../Geometry.h"

static Gizmos gizmos;

Gizmos& Gizmos::Get() { return gizmos; }

void Gizmos::AddLine(const glm::vec3& p1, const glm::vec3& p2, Color color32) {
	glm::vec3 color = Color32ToFloat(color32);
	gizmos.vertices.emplace_back(p1, color);
	gizmos.vertices.emplace_back(p2, color);
}

void Gizmos::AddBounds(const Bounds& bounds, Color color32) {
	glm::vec3 color = Color32ToFloat(color32);
	const glm::vec3& min = bounds.min, max = bounds.max;
	gizmos.vertices.emplace_back(min, color);
	gizmos.vertices.emplace_back(min.x, min.y, max.z, color);
	gizmos.vertices.emplace_back(min, color);
	gizmos.vertices.emplace_back(min.x, max.y, min.z, color);
	gizmos.vertices.emplace_back(min, color);
	gizmos.vertices.emplace_back(max.x, min.y, min.z, color);
	gizmos.vertices.emplace_back(max.x, min.y, min.z, color);
	gizmos.vertices.emplace_back(max.x, max.y, min.z, color);
	gizmos.vertices.emplace_back(max.x, min.y, min.z, color);
	gizmos.vertices.emplace_back(max.x, min.y, max.z, color);
	gizmos.vertices.emplace_back(min.x, max.y, min.z, color);
	gizmos.vertices.emplace_back(max.x, max.y, min.z, color);
	gizmos.vertices.emplace_back(min.x, max.y, min.z, color);
	gizmos.vertices.emplace_back(min.x, max.y, max.z, color);
	gizmos.vertices.emplace_back(min.x, min.y, max.z, color);
	gizmos.vertices.emplace_back(max.x, min.y, max.z, color);
	gizmos.vertices.emplace_back(min.x, min.y, max.z, color);
	gizmos.vertices.emplace_back(min.x, max.y, max.z, color);
	gizmos.vertices.emplace_back(max, color);
	gizmos.vertices.emplace_back(max.x, max.y, min.z, color);
	gizmos.vertices.emplace_back(max, color);
	gizmos.vertices.emplace_back(max.x, min.y, max.z, color);
	gizmos.vertices.emplace_back(max, color);
	gizmos.vertices.emplace_back(min.x, max.y, max.z, color);
}