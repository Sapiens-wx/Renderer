#pragma once
#include <glm/glm.hpp>

class Bounds;

class Ray {
public:
	Ray() = default;
	Ray(const glm::vec3& origin, const glm::vec3& dir) :origin(origin), dir(dir) {}
	bool Intersect(const Bounds& bounds);
	bool IntersectLineSegment(const glm::vec3& a, const glm::vec3& b, const float dist);
	// @return a normalized value indicating where the ray hits at the line (with a tolerance of dist)
	float IntersectAtLineSegment(const glm::vec3& a, const glm::vec3& b);
	// @param offset=0 a normalized offset
	// @return a point on the line indicating where the ray hits at the line (with a tolerance of dist)
	glm::vec3 IntersectPointAtLineSegment(const glm::vec3& a, const glm::vec3& b, float offset=0);
	// @return the *squared* distance of the ray and segment
	float DistSqrRaySegment(const glm::vec3& a, const glm::vec3& b);
public:
	glm::vec3 origin;
	glm::vec3 dir;
};

class Bounds {
public:
	Bounds() = default;
	Bounds(const glm::vec3& p1, const glm::vec3& p2) :min(glm::min(p1, p2)), max(glm::max(p1, p2)) {}
	inline glm::vec3 Center();
	inline glm::vec3 Size();
	void SetCenter(const glm::vec3& center);
	void SetSize(const glm::vec3& size);
	bool Intersect(const Bounds& bounds);
public:
	glm::vec3 min, max;
};

glm::vec3 Bounds::Center() {
	return (min + max) / 2.f;
}
glm::vec3 Bounds::Size() {
	return max - min;
}
