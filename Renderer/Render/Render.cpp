#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <SDL3/SDL_opengl.h>
#include <imgui.h>
#include "Render.h"
#include "Mesh.h"
#include "Shader/Shader.h"
#include "Gizmos.h"
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

void Camera::Init(int w, int h) {
	width = w;
	height = h;
	aspectRatio = float(w) / h;
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

glm::vec3 Camera::Screen2WorldPoint(int x, int y) {
	glm::vec3 forward = -transform.Forward();
	glm::vec3 right = transform.Right();
	glm::vec3 up = glm::cross(right, forward);
	//[-1,1] range
	float fx = (x << 1) / width - 1.f, fy = (y << 1) / height - 1.f;
	float tanx = glm::tan(glm::radians(fov) / 2.f);
	//actual value in the world space
	fx *= tanx * near * aspectRatio;
	fy *= tanx * near ;
	return glm::vec3(transform.position + forward * near + right * fx - up * fy);
}

Ray Camera::Screen2WorldRay(int x, int y) {
	glm::vec3 dir = glm::normalize(Screen2WorldPoint(x, y) - transform.position);
	return Ray(transform.position, dir);
}

void Renderer::Init(int width, int height)
{
	SetGLOpaque();
	this->width = width;
	this->height = height;
	lightDir = glm::vec3(1, 1, 1);
	camera.Init(width, height);
	//depth texture staff
	depthTexture.Create(width, height, GL_UNSIGNED_BYTE, nullptr, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
	//frame buffer staff
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture.GetID(), 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		CERR << "Framebuffer is not complete\n";
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::SetGLTransparent() {
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    //glDisable(GL_CULL_FACE);
    //glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
}
void Renderer::SetGLOpaque() {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
	glDisable(GL_BLEND);
}

void Renderer::Gui() {
	if (im::CollapsingHeader("Renderer")) {
		im::Indent(); camera.Gui(); im::Unindent();
		im::DragFloat3("lightDir", &lightDir.x, IM_DRAGFLOAT_SPD);
		im::SliderAngle("lightDir##Angle", &lightDir.x);
	}
}

void Renderer::Render() {
	camera.SetupVP();
	RenderObjects();
}

void Renderer::RenderDepthTexture() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);
	RenderObjects();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderMesh(const Mesh& mesh) {
	glm::mat4 model = mesh.transform.Local2World();

	// ��ȡ�� shader �е� uniform ����λ��
	const Shader* shader = mesh.shader ? mesh.shader : &Shader::GetDefaultShader();
	glUseProgram(shader->getShader());
	shader->UpdateShaderVariables(*this);
	GLuint modelLoc = glGetUniformLocation(shader->getShader(), "model");

	// ����Ⱦѭ���У����þ���
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, pos)); // vertex position
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, normal)); // vertex normal
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, uv)); // vertex uv
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	//========Render========
	// ����Ⱦѭ����ʹ�� VAO ����ɫ����Ⱦ������
	//glDrawElements(GL_TRIANGLES, mesh.tris.size(), GL_UNSIGNED_INT, 0);  // ��Ⱦ������
	glDrawElements(GL_TRIANGLES, mesh.tris.size(), GL_UNSIGNED_INT, 0);

	//disable
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	// ��� VAO
	glBindVertexArray(0);
	//delete
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}
void Renderer::RenderGizmos(Gizmos& gizmos) {
	glm::mat4 model = glm::mat4(1);

	// ��ȡ�� shader �е� uniform ����λ��
	const Shader* shader = &Shader_Unlit::Get();
	glUseProgram(shader->getShader());
	shader->UpdateShaderVariables(*this);
	GLuint modelLoc = glGetUniformLocation(shader->getShader(), "model");

	// ����Ⱦѭ���У����þ���
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//=========Buffer=========
	GLuint VBO, VAO;

	// ���� VAO��VBO �� EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// �� VAO
	glBindVertexArray(VAO);

	// �� VBO���ϴ���������
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GizmosVertex)*gizmos.vertices.size(), gizmos.vertices.data(), GL_STATIC_DRAW);

	// ���ö�������ָ��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GizmosVertex), (const GLvoid*)offsetof(GizmosVertex, pos)); // vertex position
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GizmosVertex), (const GLvoid*)offsetof(GizmosVertex, color)); // vertex normal
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//========Render========
	// ����Ⱦѭ����ʹ�� VAO ����ɫ����Ⱦ������
	//glDrawElements(GL_TRIANGLES, mesh.tris.size(), GL_UNSIGNED_INT, 0);  // ��Ⱦ������
	glDrawArrays(GL_LINES, 0, gizmos.vertices.size());

	//disable
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	// ��� VAO
	glBindVertexArray(0);
	//delete
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Camera::SetupVP() {
	// ��ͼ���������λ�ã�
	view = glm::lookAt(transform.position,   // �����λ��
		glm::vec3(0.0f, 0.0f, 0.0f),   // Ŀ���
		glm::vec3(0.0f, 1.0f, 0.0f));  // ������
	view = transform.World2Local();

	// ͶӰ����͸��ͶӰ��
	projection = glm::perspective(glm::radians(fov),   // ��Ұ�Ƕ�
		aspectRatio, // ���ڿ�߱�
		near, far); // ��ƽ����Զƽ��

	VP = projection * view;
}
