#pragma once
#include <glm/glm.hpp>
#undef near
#undef far
#include "Transform.h"

class Mesh;
typedef unsigned int GLuint;

struct Camera {
	void Init();
	void Gui();
public:
	Transform transform;
	float near, far;
	//fov in degree
	float fov;
};

class Renderer {
public:
	void Init(float aspectRatio);
	void Render();
	void RenderMesh(const Mesh& mesh);
	void Gui();
private:
	//set up view and projection matrix
	void Render_VP();
	/*
	void Render_InitBuffer();
	void Render_ReleaseBuffer();
	*/
private:
	Camera camera;
	GLuint shader;

	float aspectRatio;
	glm::mat4 view, projection;
};