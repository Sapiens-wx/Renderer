#include "Gizmos.h"

void Gizmos::AddLine(const glm::vec3& p1, const glm::vec3& p2, Color color32) {
	glm::vec3 color = Color32ToFloat(color32);
	vertices.emplace_back(p1, color);
	vertices.emplace_back(p2, color);
}