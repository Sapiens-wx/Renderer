#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#define VEC3_UP glm::vec3(0,1,0)
#define VEC3_RIGHT glm::vec3(1,0,0)
#define VEC3_FORWARD glm::vec3(0,0,1)

glm::mat4 Transform::Local2World() const {
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(rotation.y), VEC3_UP);
	model = glm::rotate(model, glm::radians(rotation.x), VEC3_RIGHT);
	model = glm::rotate(model, glm::radians(rotation.z), VEC3_FORWARD);
	return model;
}
glm::mat4 Transform::World2Local() const {
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(rotation.y), VEC3_UP);
	model = glm::rotate(model, glm::radians(rotation.x), VEC3_RIGHT);
	model = glm::rotate(model, glm::radians(rotation.z), VEC3_FORWARD);
	return glm::inverse(model);
}

void Transform::Gui() {
	if (ImGui::CollapsingHeader("transform", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat3("position", &position.x, .1f);
		ImGui::DragFloat3("rotation", &rotation.x, .1f);
	}
}