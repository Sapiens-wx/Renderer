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

	// 获取在 shader 中的 uniform 变量位置
	GLuint modelLoc = glGetUniformLocation(shader, "model");
	GLuint viewLoc = glGetUniformLocation(shader, "view");
	GLuint projectionLoc = glGetUniformLocation(shader, "projection");

	// 在渲染循环中，设置矩阵
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//=========Buffer=========
	GLuint VBO, VAO, EBO;

	// 创建 VAO、VBO 和 EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// 绑定 VAO
	glBindVertexArray(VAO);

	// 绑定 VBO，上传顶点数据
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);

	// 绑定 EBO，上传索引数据
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*mesh.tris.size(), mesh.tris.data(), GL_STATIC_DRAW);

	// 设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0); // vertex position
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0); // vertex position
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0); // vertex position
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	//========Render========
	// 在渲染循环中使用 VAO 和着色器渲染立方体
	glDrawElements(GL_TRIANGLES, mesh.tris.size(), GL_UNSIGNED_INT, 0);  // 渲染立方体

	//disable
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	// 解绑 VAO
	glBindVertexArray(0);
}

void Renderer::Render_VP() {
	// 视图矩阵（摄像机位置）
	view = glm::lookAt(camera.transform.position,   // 摄像机位置
		glm::vec3(0.0f, 0.0f, 0.0f),   // 目标点
		glm::vec3(0.0f, 1.0f, 0.0f));  // 上向量
	view = camera.transform.World2Local();

	// 投影矩阵（透视投影）
	projection = glm::perspective(glm::radians(camera.fov),   // 视野角度
		aspectRatio, // 窗口宽高比
		camera.near, camera.far); // 近平面与远平面
}
void Renderer::Render_Light() {
	GLint lightLoc = glGetUniformLocation(shader, "lightDir");
	glUniform3fv(lightLoc, 1, &lightDir.x);
}
