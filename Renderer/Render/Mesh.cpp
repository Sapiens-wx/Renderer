#include "Mesh.h"
#include "../def.h"
#include <imgui.h>

namespace im = ImGui;

bool EndsWith(const char* str1, const char* str2) {
	size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    if (len2 > len1) return false;
    return strcmp(str1 + (len1 - len2), str2) == 0;
}

static int counter = 0;

Mesh::Mesh(): name(std::move(std::to_string(counter++))), shader(nullptr)
{}

void Mesh::Gui() {
	if (im::CollapsingHeader(name.c_str())) {
		im::PushID(name.c_str());
		im::Indent();
		transform.Gui();
		im::Unindent();
		im::PopID();
	}
}

void Mesh::LoadFromFile(const char* path) {
	std::ifstream stream(path);
	if (!stream.is_open()) {
		CERR << "cannot open file " << path << '\n';
		stream.close();
		return;
	}
	if (EndsWith(path, ".obj"))
		LoadFromFile_Obj(stream);
	stream.close();
}

void Mesh::LoadFromFile_Obj(std::ifstream& file) {
	std::string line;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	while (std::getline(file, line)) {
		float x, y, z;
		if (line.size()&&line[0] == 'v') {
			if (line[1] == 'n') { //normal
				if (sscanf_s(line.c_str() + 2, "%f%f%f", &x, &y, &z) != 3) {
					CERR << "cannot parse the obj file\n";
					return;
				}
				normals.emplace_back(x,y,z);
			}
			else if (line[1] == 't') { //uv
				if (sscanf_s(line.c_str() + 2, "%f%f", &x, &y) != 2) {
					CERR << "cannot parse the obj file\n";
					return;
				}
				uvs.emplace_back(x,y);
			}
			else { //vertex position
				if (sscanf_s(line.c_str() + 1, "%f%f%f", &x, &y, &z) != 3) {
					CERR << "cannot parse the obj file\n";
					return;
				}
				vertices.emplace_back();
				vertices.back().pos = glm::vec3(x, y, z);
			}
		}
		else if (line.size() && line[0] == 'f') {// faces
			int v1, t1, n1, v2, t2, n2, v3, t3, n3, v4, t4, n4;
			int scanfResult;
			if ((scanfResult=sscanf_s(line.c_str() + 1, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3, &v4, &t4, &n4)) < 9) {
				CERR << "cannot parse the obj file. Currently does not support face lines other than 'f x/x/x x/x/x x/x/x'\n";
				return;
			}
			v1 -= 1; v2 -= 1; v3 -= 1; v4 -= 1; t1 -= 1; t2 -= 1; t3 -= 1; t4 -= 1; n1 -= 1; n2 -= 1; n3 -= 1; n4 -= 1; //deal with the fucking index starting from 1
			tris.push_back(v1);
			tris.push_back(v2);
			tris.push_back(v3);
			vertices[v1].uv = uvs[t1];
			vertices[v1].normal = normals[n1];
			vertices[v2].uv = uvs[t2];
			vertices[v2].normal = normals[n2];
			vertices[v3].uv = uvs[t3];
			vertices[v3].normal = normals[n3];
			if (scanfResult > 9) { //this line has four vertices
				tris.push_back(v1);
				tris.push_back(v3);
				tris.push_back(v4);
				vertices[v4].uv = uvs[t4];
				vertices[v4].normal = normals[n4];
			}
		}
	}
}