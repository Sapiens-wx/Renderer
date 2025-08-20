#pragma once
#include <SDL3/SDL_opengl.h>

class RenderTexture {
public:
	RenderTexture();
	~RenderTexture();
	void Swap(RenderTexture& other);
	/* calls glGenTextures if needed
	 * then calls glTexImage2D
	*/
	void Create(int width, int height, GLenum valueType=GL_UNSIGNED_BYTE, const void* pixels=nullptr, GLint internalFormat=GL_RGBA, GLenum format=GL_RGBA);
	/* calls glTexSubImage2D to update the texture
	*/
	void UpdateImage(int x, int y, int width, int height, GLint format, GLenum valueType, const void* pixels);
	void Release();
	//gets the id of the texture
	inline GLuint GetID() const { return id; }
	inline int GetWidth() const { return w; }
	inline int GetHeight() const { return h; }
	inline bool IsCreated() const { return id != 0; }
private:
	int w, h;
	GLuint id;
};

class Texture {
	typedef unsigned char u8;
	typedef unsigned int u32;
	typedef u32 GLuint;
public:
	Texture();
	~Texture();
	Texture(const char* path);
	//loads the pixels from the given file to [data]
	void LoadFromFile(const char* path);
	//uploads [data] to gpu
	//auto-determines calling RenderTexture::Create or ::UpdateImage
	void UploadTexture();
	inline const u32* GetData() const { return data; }
	inline int GetWidth() const { return w; }
	inline int GetHeight() const { return h; }
	inline GLuint GetID() const { return renderTex.GetID(); }
public:
	RenderTexture renderTex;
private:
	u32* data;
	int w, h;
};
