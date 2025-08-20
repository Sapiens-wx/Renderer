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
	frameTexture.Create(width, height, GL_UNSIGNED_BYTE, nullptr, GL_RGBA, GL_RGBA);
	BindToDepthBuffer();
	BindToFrameBuffer();
	CreateFrameDepthBuffer();
	//shader
	shader_unlit.Load();
	shader_unlit.texture = &frameTexture;
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
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferLoc);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameTexture.GetID(), 0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	RenderObjects();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	PostProcessing();
	Blit(0, &frameTexture, shader_unlit);
}

void Renderer::RenderDepthTexture() {
	glBindFramebuffer(GL_FRAMEBUFFER, depthBufferLoc);
	glClear(GL_DEPTH_BUFFER_BIT);
	RenderObjects();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderMesh(const Mesh& mesh) {
	glm::mat4 model = mesh.transform.Local2World();

	// 获取在 shader 中的 uniform 变量位置
	const Shader* shader = mesh.shader ? mesh.shader : &Shader::GetDefaultShader();
	glUseProgram(shader->getShader());
	shader->UpdateShaderVariables(*this);
	GLuint modelLoc = glGetUniformLocation(shader->getShader(), "model");

	// 在渲染循环中，设置矩阵
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, pos)); // vertex position
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, normal)); // vertex normal
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, uv)); // vertex uv
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	//========Render========
	// 在渲染循环中使用 VAO 和着色器渲染立方体
	//glDrawElements(GL_TRIANGLES, mesh.tris.size(), GL_UNSIGNED_INT, 0);  // 渲染立方体
	glDrawElements(GL_TRIANGLES, mesh.tris.size(), GL_UNSIGNED_INT, 0);

	//disable
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	// 解绑 VAO
	glBindVertexArray(0);
	//delete
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}
void Renderer::RenderGizmos(Gizmos& gizmos) {
	glm::mat4 model = glm::mat4(1);

	// 获取在 shader 中的 uniform 变量位置
	const Shader* shader = &Shader_Unlit::Get();
	glUseProgram(shader->getShader());
	shader->UpdateShaderVariables(*this);
	GLuint modelLoc = glGetUniformLocation(shader->getShader(), "model");

	// 在渲染循环中，设置矩阵
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//=========Buffer=========
	GLuint VBO, VAO;

	// 创建 VAO、VBO 和 EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// 绑定 VAO
	glBindVertexArray(VAO);

	// 绑定 VBO，上传顶点数据
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GizmosVertex)*gizmos.vertices.size(), gizmos.vertices.data(), GL_STATIC_DRAW);

	// 设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GizmosVertex), (const GLvoid*)offsetof(GizmosVertex, pos)); // vertex position
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GizmosVertex), (const GLvoid*)offsetof(GizmosVertex, color)); // vertex normal
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//========Render========
	// 在渲染循环中使用 VAO 和着色器渲染立方体
	//glDrawElements(GL_TRIANGLES, mesh.tris.size(), GL_UNSIGNED_INT, 0);  // 渲染立方体
	glDrawArrays(GL_LINES, 0, gizmos.vertices.size());

	//disable
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	// 解绑 VAO
	glBindVertexArray(0);
	//delete
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Camera::SetupVP() {
	// 视图矩阵（摄像机位置）
	view = glm::lookAt(transform.position,   // 摄像机位置
		glm::vec3(0.0f, 0.0f, 0.0f),   // 目标点
		glm::vec3(0.0f, 1.0f, 0.0f));  // 上向量
	view = transform.World2Local();

	// 投影矩阵（透视投影）
	projection = glm::perspective(glm::radians(fov),   // 视野角度
		aspectRatio, // 窗口宽高比
		near, far); // 近平面与远平面

	VP = projection * view;
	
	//corner points
	glm::vec3 center = -transform.Forward() * near*1.0001f + transform.position;
	const glm::vec3 right = transform.Right(), up = transform.Up();
	float halfH = glm::tan(glm::radians(fov) / 2) * near, halfW=halfH*aspectRatio;
	glm::vec3 maxx=right*halfW, maxy=up*halfH;
	leftTop = center - maxx + maxy;
	leftBot = center - maxx - maxy;
	rightTop = center + maxx + maxy;
	rightBot = center + maxx - maxy;
}

void Renderer::Blit(RenderTexture* dst, RenderTexture* src, const Shader& shader) {
	const constexpr glm::mat4 mat4Identity = glm::mat4(1);

	// 获取在 shader 中的 uniform 变量位置
	glUseProgram(shader.getShader());
	shader.UpdateShaderVariables(*this);
	if (dst) {
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferLoc);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dst->GetID(), 0);
	}
	if (src) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, src->GetID());
	}
	GLuint modelLoc = glGetUniformLocation(shader.getShader(), "model");

	// 在渲染循环中，设置矩阵
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mat4Identity));

	//
	Vertex vertices[] = {
		{camera.leftTop, {0,1}},
		{camera.rightBot, {1,0}},
		{camera.rightTop,  {1,1}},
		{camera.leftTop, {0,1}},
		{camera.leftBot, {0,0}},
		{camera.rightBot, {1,0}}
	};

	//=========Buffer=========
	GLuint VBO, VAO;

	// 创建 VAO、VBO 和 EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// 绑定 VAO
	glBindVertexArray(VAO);

	// 绑定 VBO，上传顶点数据
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, pos)); // vertex position
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, normal)); // vertex uv
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, uv)); // vertex uv
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	//========Render========
	// 在渲染循环中使用 VAO 和着色器渲染立方体
	glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices)/sizeof(Vertex));

	//disable
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// 解绑 VAO
	glBindVertexArray(0);
	//delete
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Renderer::BindToFrameBuffer() {
	glGenFramebuffers(1, &frameBufferLoc);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferLoc);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameTexture.GetID(), 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		CERR << "Framebuffer is not complete\n";
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::BindToDepthBuffer() {
	glGenFramebuffers(1, &depthBufferLoc);
	glBindFramebuffer(GL_FRAMEBUFFER, depthBufferLoc);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture.GetID(), 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		CERR << "Framebuffer is not complete\n";
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::CreateFrameDepthBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferLoc);
	glGenRenderbuffers(1, &frameDepthBufferLoc);
	glBindRenderbuffer(GL_RENDERBUFFER, frameDepthBufferLoc);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, frameDepthBufferLoc);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Renderer::~Renderer() {
	if (frameDepthBufferLoc)
		glDeleteRenderbuffers(1, &frameDepthBufferLoc);
	if (frameBufferLoc)
		glDeleteFramebuffers(1, &frameBufferLoc);
	if (depthBufferLoc)
		glDeleteFramebuffers(1, &depthBufferLoc);
}
