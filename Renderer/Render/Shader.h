#pragma once

typedef unsigned int GLenum;
typedef unsigned int GLuint;

class Shader {
public:
	Shader();
	void initFromMemory(const char* vtxShader, const char* frgShader);
	void initFromFile(const char* vtxShader, const char* frgShader);
	GLuint getShader() const;
	static GLuint GetDefaultShader();
private:
	//compiles a single vertex/fragment shader
	static GLuint compileShader(const char* str, GLenum type);
	static GLuint compileShaderFromFile(const char* file, GLenum type);
	//compiles vertex and fragment shader into a shader program
	static GLuint compileShaders(GLuint vertexShader, GLuint fragmentShader);
private:
	GLuint shaderProgram;
};