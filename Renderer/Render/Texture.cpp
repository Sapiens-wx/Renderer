#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include <stb_image.h>
#include <string.h>
//#include <GL/glew.h>
#include <SDL3/SDL_opengl.h>
#include "../def.h"

Texture::Texture() :data(nullptr), texID(0)
{}
Texture::Texture(const char* path) :data(nullptr), texID(0)
{
    LoadFromFile(path);
}
Texture::~Texture() {
    if (data)
        free(data);
    if (texID)
        glDeleteTextures(1, &texID);
}

void Texture::GLGenTexture() {
    if (texID != 0) {
        CERR << "texture already generated\n";
        return;
    }
    glGenTextures(1, &texID);
    if (texID == 0)
        CERR << "fail to glGenTextures\n";
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
    data = (u32*)d;
}

void Texture::UploadTexture(const GLuint textureID) {
    if (data == nullptr) {
        CERR << "trying to upload null texture\n";
        return;
    }
    if (textureID == 0) {
        CERR << "textureID is 0\n";
        return;
    }
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //set filtering values
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
}