#include "Geometry.h"

bool Ray::Intersect(const Bounds& bounds) {
	float tmin = (bounds.min.x - origin.x) / dir.x;
    float tmax = (bounds.max.x - origin.x) / dir.x;

    if (tmin > tmax) std::swap(tmin, tmax); // ȷ�� tmin <= tmax

    float tymin = (bounds.min.y - origin.y) / dir.y;
    float tymax = (bounds.max.y - origin.y) / dir.y;

    if (tymin > tymax) std::swap(tymin, tymax); // ȷ�� tymin <= tymax

    if (tmin > tymax || tymin > tmax)
        return false; // û���ཻ

    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;

    float tzmin = (bounds.min.z - origin.z) / dir.z;
    float tzmax = (bounds.max.z - origin.z) / dir.z;

    if (tzmin > tzmax) std::swap(tzmin, tzmax); // ȷ�� tzmin <= tzmax

    if (tmin > tzmax || tzmin > tmax)
        return false; // û���ཻ

    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;

    return tmax >= 0; // tmax >= 0 ��ʾ���ߺͰ�Χ���ཻ�����߷���Ϊ��
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