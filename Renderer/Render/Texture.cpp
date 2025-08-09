#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include <stb_image.h>
#include <string.h>
#include <SDL3/SDL_opengl.h>
#include "../def.h"

Texture::Texture() :data(nullptr)
{}
Texture::Texture(const char* path) :data(nullptr)
{
    LoadFromFile(path);
}
Texture::~Texture() {
    if (data)
        free(data);
}

void Texture::LoadFromFile(const char* path) {
    if (data) {
        free(data);
        data = nullptr;
    }
    int channels;
    u8* d = stbi_load(path, &w, &h, &channels, 4);
    if (d == nullptr)
        CERR << "cannot load texture from " << path << '\n';
    else {
		data = (u32*)d;
        UploadTexture();
    }
}

void Texture::UploadTexture() {
    if (data == nullptr) {
        CERR << "uploading a texture with nullptr to gpu\n";
        return;
    }
    if (renderTex.IsCreated())
        renderTex.UpdateImage(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
    else
        renderTex.Create(w, h, GL_UNSIGNED_BYTE, data, GL_RGBA, GL_RGBA);
}

RenderTexture::RenderTexture():id(0)
{}

RenderTexture::~RenderTexture() {
    Release();
}

void RenderTexture::Create(int width, int height, GLenum valueType, const void* pixels, GLint internalFormat, GLenum format) {
    if (id == 0)
        glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, valueType, pixels);
}

void RenderTexture::UpdateImage(int x, int y, int width, int height, GLint format, GLenum valueType, const void* pixels) {
    if (id == 0)
        CERR << "trying to update image of a texture with id=0\n";
    glBindTexture(GL_TEXTURE_2D, id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, format, valueType, pixels);
}

void RenderTexture::Release() {
    if(id)
		glDeleteTextures(1, &id);
    id = 0;
}