#include "Render.h"
#include "Mesh.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <SDL3/SDL_opengl.h>
#include <imgui.h>
#undef near
#undef far
#include "../def.h"

#pragma region printing
inline std::ostream& operator<<(std::ostream& stream, glm::vec3 vec) {
	return stream << '(' << vec.x << ',' << vec.y << ',' << vec.z << ')';
}
inline std::ostream& operator<<(std::ostream& stream, glm::vec4 vec) {
	return stream << '(' << vec.x << ',' << vec.y << ',' << vec.z << ',' << vec.w << ')';
}
inline std::ostream& operator<<(std::ostream& stream, glm::mat4 mat) {
	int bufSize=snprintf(0, 0, "%8.3f,%8.3f,%8.3f,%8.3f,\n%8.3f,%8.3f,%8.3f,%8.3f,\n%8.3f,%8.3f,%8.3f,%8.3f,\n%8.3f,%8.3f,%8.3f,%8.3f,\n",
		mat[0][0], mat[1][0], mat[2][0], mat[3][0],
		mat[0][1], mat[1][1], mat[2][1], mat[3][1],
		mat[0][2], mat[1][2], mat[2][2], mat[3][2],
		mat[0][3], mat[1][3], mat[2][3], mat[3][3]
		)+1;
	char* buf = new char[bufSize];
	snprintf(buf, bufSize, "%8.3f,%8.3f,%8.3f,%8.3f,\n%8.3f,%8.3f,%8.3f,%8.3f,\n%8.3f,%8.3f,%8.3f,%8.3f,\n%8.3f,%8.3f,%8.3f,%8.3f,\n",
		mat[0][0], mat[1][0], mat[2][0], mat[3][0],
		mat[0][1], mat[1][1], mat[2][1], mat[3][1],
		mat[0][2], mat[1][2], mat[2][2], mat[3][2],
		mat[0][3], mat[1][3], mat[2][3], mat[3][3]
		);
	stream << buf;
	delete[] buf;
	return stream;
}
#pragma endregion

void Camera::Init() {
	fov = 90;
	near = 0.1;
	far = 1000;
	transform.position = glm::vec3(0, 0, 10);
	transform.SetRotation(glm::vec3(0, 0, 0));
}

namespace im = ImGui;
void Camera::Gui() {
	if (im::CollapsingHeader("camera")) {
		im::Indent(); transform.Gui(); im::Unindent();
		im::DragFloat("fov", &fov, IM_DRAGFLOAT_SPD, 0, 360);
		im::DragFloat("near", &near, IM_DRAGFLOAT_SPD);
		im::SameLine(); im::DragFloat("far", &far, IM_DRAGFLOAT_SPD);
	}
}

void Renderer::Init(float aspectRatio)
{
	this->aspectRatio = aspectRatio;
	shader = Shader::GetDefaultShader();
	camera.Init();
}

void Renderer::Gui() {
	if (im::CollapsingHeader("Renderer")) {
		im::Indent(); camera.Gui(); im::Unindent();
		im::DragFloat3("lightDir", &lightDir.x, IM_DRAGFLOAT_SPD);
	}
}

void Renderer::Render() {
	glUseProgram(shader);
	Render_VP();
	Render_Light();
}

void Renderer::RenderMesh(const Mesh& mesh) {
	glm::mat4 model = mesh.transform.Local2World();

	// ��ȡ�� shader �е� uniform ����λ��
	GLuint modelLoc = glGetUniformLocation(shader, "model");
	GLuint viewLoc = glGetUniformLocation(shader, "view");
	GLuint projectionLoc = glGetUniformLocation(shader, "projection");

	// ����Ⱦѭ���У����þ���
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//=========Buffer=========
	GLuint VBO, VAO, EBO;

	// ���� VAO��VBO �� EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// �� VAO
	glBindVertexArray(VAO);

	// �� VBO���ϴ���������
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);

	// �� EBO���ϴ���������
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*mesh.tris.size(), mesh.tris.data(), GL_STATIC_DRAW);

	// ���ö�������ָ��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0); // vertex position
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0); // vertex position
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0); // vertex position
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	//========Render========
	// ����Ⱦѭ����ʹ�� VAO ����ɫ����Ⱦ������
	glDrawElements(GL_TRIANGLES, mesh.tris.size(), GL_UNSIGNED_INT, 0);  // ��Ⱦ������

	//disable
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	// ��� VAO
	glBindVertexArray(0);
}

void Renderer::Render_VP() {
	// ��ͼ���������λ�ã�
	view = glm::lookAt(camera.transform.position,   // �����λ��
		glm::vec3(0.0f, 0.0f, 0.0f),   // Ŀ���
		glm::vec3(0.0f, 1.0f, 0.0f));  // ������
	view = camera.transform.World2Local();

	// ͶӰ����͸��ͶӰ��
	projection = glm::perspective(glm::radians(camera.fov),   // ��Ұ�Ƕ�
		aspectRatio, // ���ڿ�߱�
		camera.near, camera.far); // ��ƽ����Զƽ��
}
void Renderer::Render_Light() {
	GLint lightLoc = glGetUniformLocation(shader, "lightDir");
	glUniform3fv(lightLoc, 1, &lightDir.x);
}
