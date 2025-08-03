#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform {
public:
	void Gui();
	inline glm::mat4 GetRotationMat() const;
	//get rotation in degrees
	inline glm::vec3 GetRotationVec() const;
	//set rotation in degrees
	inline void SetRotation(const glm::vec3& rot);

	glm::mat4 Local2World() const;
	glm::mat4 World2Local() const;
	inline glm::vec3 Right() const;
	inline glm::vec3 Forward() const;
	inline glm::vec3 Up() const;
public:
	glm::vec3 position;
	glm::quat rotation;
public:
	static const glm::vec3 up, down, right, left, forward, backward;
};

glm::mat4 Transform::GetRotationMat() const {
	return glm::mat4_cast(rotation);
}
glm::vec3 Transform::GetRotationVec() const {
	return glm::degrees(glm::eulerAngles(rotation));
}
void Transform::SetRotation(const glm::vec3& rot) {
	rotation = glm::quat(glm::radians(rot));
}
glm::vec3 Transform::Right() const {
	return rotation * right;
}
glm::vec3 Transform::Forward() const {
	return rotation * forward;
}
glm::vec3 Transform::Up() const {
	return rotation * up;
}