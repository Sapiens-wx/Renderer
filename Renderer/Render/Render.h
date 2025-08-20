#pragma once
#include <glm/glm.hpp>
#include <functional>
#include "Transform.h"
#include "Texture.h"
#include "Shader/Shader.h"
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
	//* set up view and projection matrix
	//* update leftTop, leftBot, rightTop, rightBot of the screen
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
	glm::vec3 leftTop, leftBot, rightTop, rightBot;
};

class Renderer {
	friend Shader;
public:
	~Renderer();
	void Init(int width, int height);
	void SetGLTransparent();
	void SetGLOpaque();
	//Renders the scene every frame. will use RenderObjects() to render the meshes.
	void Render();
	void RenderMesh(const Mesh& mesh);
	void RenderGizmos(Gizmos& gizmos);
	void RenderDepthTexture();
	void Gui();
	void Blit(RenderTexture* dst, RenderTexture* src, const Shader& shader);
	static Renderer& Get();
private:
	//generates a frame buffer and binds frameTexture to it
	void BindToFrameBuffer();
	//generates a frame buffer and binds depthTexture to it
	void BindToDepthBuffer();
	//creates the frame's depth buffer
	void CreateFrameDepthBuffer();
public:
	Camera camera;
	glm::vec3 lightDir;
	RenderTexture depthTexture, frameTexture;
	//user-defined rendering logic for rendering meshes
	std::function<void(void)> RenderObjects;
	/* user-defined post-process logic
	 * the final result must be stored in [frameTexture]
	*/
	std::function<void(void)> PostProcessing;
private:
	//for rendering the frameTexture to the screen
	Shader_Unlit_Texture shader_unlit;
	int width, height;
	float aspectRatio;
	GLuint depthBufferLoc, frameBufferLoc, frameDepthBufferLoc;
};