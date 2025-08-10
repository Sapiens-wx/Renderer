#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/quaternion.hpp>
#include <imgui.h>
#include "../def.h"

const glm::vec3 Transform::up = {0,1,0}, Transform::down = {0,-1,0}, Transform::right = {1,0,0}, Transform::left = {-1,0,0}, Transform::forward = {0,0,1}, Transform::backward = {0,0,-1};

glm::mat4 Transform::Local2World() const {
	glm::mat4 model = glm::mat4(1);
	model[0].x = scale.x;
	model[1].y = scale.y;
	model[2].z = scale.z;
	model = GetRotationMat() * model;
	model[3] = glm::vec4(position, 1);
	return model;
}
glm::mat4 Transform::World2Local() const {
	glm::mat4 model = glm::mat4(1);
	model[0].x = scale.x;
	model[1].y = scale.y;
	model[2].z = scale.z;
	model = GetRotationMat() * model;
	model[3] = glm::vec4(position, 1);
	return glm::inverse(model);
}

void Transform::Gui() {
	if (ImGui::CollapsingHeader("transform", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat3("position", &position.x, IM_DRAGFLOAT_SPD);
		ImGui::DragFloat3("scale", &scale.x, IM_DRAGFLOAT_SPD);
		glm::vec3 eularAngles = GetRotationVec();
		if (ImGui::DragFloat3("rotation", &eularAngles.x, IM_DRAGFLOAT_SPD))
			SetRotation(eularAngles);
	}
}
