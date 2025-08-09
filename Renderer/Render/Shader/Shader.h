#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "../Texture.h"

typedef unsigned int GLenum;
typedef unsigned int GLuint;
class Renderer;

class Shader {
public:
	Shader();
	void initFromMemory(const char* vtxShader, const char* frgShader);
	void initFromFile(const char* vtxShader, const char* frgShader);
	//gets the shader id
	GLuint getShader() const;

	//update the uniform variables of the shader
	virtual void UpdateShaderVariables(const Renderer& renderer) const;
	//on gui
	virtual void OnGui() {};
	//each derived class of Shader has a default path. loads the shader from the default path
	virtual void Load() {};

	static const Shader& GetDefaultShader();
protected:
	typedef Shader Base;
protected:
	//compiles a single vertex/fragment shader
	static GLuint compileShader(const char* str, GLenum type);
	static GLuint compileShaderFromFile(const char* file, GLenum type);
	//compiles vertex and fragment shader into a shader program
	static GLuint compileShaders(GLuint vertexShader, GLuint fragmentShader);
protected:
	GLuint shaderProgram;
};

class Shader_BlinnPhong : public Shader {
public:
	Shader_BlinnPhong();
	void UpdateShaderVariables(const Renderer& renderer) const override;
	void OnGui() override;
	void Load() override;
	RenderTexture* tex;
private:
	mutable float roughness; mutable GLuint roughnessLoc;
	mutable glm::vec3 diffuseColor, specularColor; mutable GLuint diffuseLoc, specularLoc;
};

class Shader_Unlit : public Shader {
public:
	static Shader_Unlit& Get();
	void UpdateShaderVariables(const Renderer& renderer) const override;
	void Load() override;
private:
	static std::unique_ptr<Shader_Unlit> inst;
};

class Shader_Unlit_Texture : public Shader {
public:
	Shader_Unlit_Texture();
	void UpdateShaderVariables(const Renderer& renderer) const override;
	void Load() override;
public:
	RenderTexture* texture;
};
