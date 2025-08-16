#pragma once
#include <glm/glm.hpp>
#include <functional>
#include "Transform.h"
#include "Texture.h"
#include "../Geometry.h"
#undef near
#undef far

class Mesh;
class Shader;
class Gizmos;
typedef unsigned int GLuint;

struct Camera {
	void Init(int w, int h);
	void Gui();
	//set up view and projection matrix
	void SetupVP();
	glm::vec3 Screen2WorldPoint(int x, int y);
	Ray Screen2WorldRay(int x, int y);
public:
	Transform transform;
	float near, far;
	//fov in degree
	float fov;
	float width, height, aspectRatio;
	glm::mat4 view, projection, VP;
};

class Renderer {
	friend Shader;
public:
	void Init(int width, int height);
	void SetGLTransparent();
	void SetGLOpaque();
	//Renders the scene every frame. will use RenderObjects() to render the meshes.
	void Render();
	void RenderMesh(const Mesh& mesh);
	void RenderGizmos(Gizmos& gizmos);
	void RenderDepthTexture();
	void Gui();
	static Renderer& Get();
private:
	/*
	void Render_InitBuffer();
	void Render_ReleaseBuffer();
	*/
public:
	Camera camera;
	glm::vec3 lightDir;
	RenderTexture depthTexture;
	//user-defined rendering logic for rendering meshes
	std::function<void(void)> RenderObjects;
private:
	GLuint shader;

	int width, height;
	float aspectRatio;

	GLuint framebuffer;
};