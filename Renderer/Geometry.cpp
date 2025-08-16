#include "Geometry.h"


float Ray::DistSqrRaySegment(const glm::vec3& a, const glm::vec3& b) {
    glm::vec3 u = b - a;
    glm::vec3 v = dir; // 可以提前归一化
    glm::vec3 w0 = a - origin;

    float A = glm::dot(u, u);
    float B = glm::dot(u, v);
    float C = glm::dot(v, v);
    float D = glm::dot(u, w0);
    float E = glm::dot(v, w0);

    float denom = A * C - B * B;

    float s, t;

    if (denom < 1e-9) {
        // 方向几乎平行
        s = 0;
        t = E / C;
    } else {
        s = (B * E - C * D) / denom;
        t = (A * E - B * D) / denom;
    }

    // 约束 s 在 [0, 1]
    if (s < 0) {
        s = 0;
        t = E / C;
    } else if (s > 1) {
        s = 1;
        t = (E + B) / C;
    }

    // 约束 t >= 0
    if (t < 0) {
        t = 0;
        // 退化为点-线段距离
        glm::vec3 w = origin - a;
        float proj = glm::dot(w, u) / A;
        proj = glm::clamp(proj, 0.0f, 1.0f);
        glm::vec3 closestSeg = a + u * proj;
        closestSeg -= origin;
        return glm::dot(closestSeg, closestSeg);
    }

    glm::vec3 closestSeg = a + u * s;
    glm::vec3 closestRay = origin + v * t;
    closestSeg -= closestRay;
	return glm::dot(closestSeg, closestSeg);
}
bool Ray::Intersect(const Bounds& bounds) {
	float tmin = (bounds.min.x - origin.x) / dir.x;
    float tmax = (bounds.max.x - origin.x) / dir.x;

    if (tmin > tmax) std::swap(tmin, tmax); // 确保 tmin <= tmax

    float tymin = (bounds.min.y - origin.y) / dir.y;
    float tymax = (bounds.max.y - origin.y) / dir.y;

    if (tymin > tymax) std::swap(tymin, tymax); // 确保 tymin <= tymax

    if (tmin > tymax || tymin > tmax)
        return false; // 没有相交

    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;

    float tzmin = (bounds.min.z - origin.z) / dir.z;
    float tzmax = (bounds.max.z - origin.z) / dir.z;

    if (tzmin > tzmax) std::swap(tzmin, tzmax); // 确保 tzmin <= tzmax

    if (tmin > tzmax || tzmin > tmax)
        return false; // 没有相交

    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;

    return tmax >= 0; // tmax >= 0 表示射线和包围盒相交且射线方向为正
}

bool Ray::IntersectLineSegment(const glm::vec3& a, const glm::vec3& b, const float dist) {
    return DistSqrRaySegment(a, b) <= dist;
}

float Ray::IntersectAtLineSegment(const glm::vec3& a, const glm::vec3& b) {
    const constexpr float epsilon = 0.0001f;
    glm::vec3 lineDir = b - a;
    float lineDirMagSqr = glm::dot(lineDir, lineDir);
    glm::vec3 v0 = a - origin;
    glm::vec3 v1 = glm::cross(dir, glm::cross(dir, lineDir / std::sqrtf(lineDirMagSqr)));
    float v1MagSqr = glm::dot(v1, v1);
    if (v1MagSqr < epsilon) { //parallel
        return 0;
    }
    v1 /= std::sqrtf(v1MagSqr);
    return glm::dot(v0, v1) / glm::dot(lineDir, v1);
}
glm::vec3 Ray::IntersectPointAtLineSegment(const glm::vec3& a, const glm::vec3& b) {
    const constexpr float epsilon = 0.0001f;
    glm::vec3 lineDir = b - a;
    float lineDirMagSqr = glm::dot(lineDir, lineDir);
    glm::vec3 v0 = a - origin;
    glm::vec3 v1 = glm::cross(dir, glm::cross(dir, lineDir / std::sqrtf(lineDirMagSqr)));
    float v1MagSqr = glm::dot(v1, v1);
    if (v1MagSqr < epsilon) { //parallel
        return a;
    }
    v1 /= std::sqrtf(v1MagSqr);
    float t = glm::dot(v0, v1) / glm::dot(lineDir, v1);
    return a + t * lineDir;
}

bool Bounds::Intersect(const Bounds& other) {
	if (max.x < other.min.x || min.x > other.max.x)
		return false;

	if (max.y < other.min.y || min.y > other.max.y)
		return false;

	if (max.z < other.min.z || min.z > other.max.z)
		return false;

	return true;
}

void Bounds::SetCenter(const glm::vec3& center) {
	glm::vec3 offset = center - Center();
	min += offset;
	max += offset;
}

void Bounds::SetSize(const glm::vec3& size) {
	glm::vec3 offset = (size - Size())/2.f;
	min -= offset;
	max += offset;
}