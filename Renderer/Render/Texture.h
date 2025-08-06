#pragma once

class Texture {
	typedef unsigned char u8;
	typedef unsigned int u32;
	typedef u32 GLuint;
public:
	Texture();
	~Texture();
	Texture(const char* path);
	void LoadFromFile(const char* path);
	//uploads the texture to the given textureID. Should be called once.
	void UploadTexture(const GLuint textureID);
	//generates a texture by calling glGenTextures.
	void GLGenTexture();
public:
	int w, h;
	u32* data;
	GLuint texID;
};
