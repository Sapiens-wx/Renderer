#include "Mesh.h"
#include <imgui.h>

namespace im = ImGui;

static int counter = 0;

Mesh::Mesh(): name(std::move(std::to_string(counter)))
{}

void Mesh::Gui() {
	if (im::CollapsingHeader(name.c_str())) {
		im::PushID(name.c_str());
		im::Indent();
		if (im::CollapsingHeader("transform")) {
			im::DragFloat3("position", &transform.position.x, .1f);
			im::DragFloat3("rotation", &transform.rotation.x, .1f);
		}
		im::Unindent();
		im::PopID();
	}
}