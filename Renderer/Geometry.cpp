#include "Geometry.h"

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

bool Bounds::Intersect(const Bounds& other) {
	if (max.x < other.min.x || min.x > other.max.x)
		return false;

	if (max.y < other.min.y || min.y > other.max.y)
		return false;

	if (max.z < other.min.z || min.z > other.max.z)
		return false;

	return true;
}