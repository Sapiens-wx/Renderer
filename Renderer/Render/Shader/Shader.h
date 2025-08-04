#pragma once
#include <glm/glm.hpp>

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
	//compiles a single vertex/fragment shader
	static GLuint compileShader(const char* str, GLenum type);
	static GLuint compileShaderFromFile(const char* file, GLenum type);
	//compiles vertex and fragment shader into a shader program
	static GLuint compileShaders(GLuint vertexShader, GLuint fragmentShader);
protected:
	GLuint shaderProgram;
};

class Shader_BlinnPhong : public Shader {
	typedef Shader Super;
public:
	void UpdateShaderVariables(const Renderer& renderer) const override;
	void OnGui() override;
	void Load() override;
private:
	mutable float roughness; mutable GLuint roughnessLoc;
	mutable glm::vec3 diffuseColor, specularColor; mutable GLuint diffuseLoc, specularLoc;
};
