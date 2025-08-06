#include <GL/glew.h>
#include <SDL3/SDL_opengl.h>
#include <stdio.h>
#include <imgui.h>
#include "Shader.h"
#include "../Render.h"
#include "../../def.h"
#pragma warning(push)
#pragma warning(disable: 4996)
//used in UpdateShaderVariables. only get the location from gpu if [uint_loc] is 0
#define InitLoc(uint_loc, str_name) do{\
    if(uint_loc==0)\
        uint_loc = glGetUniformLocation(shaderProgram, str_name);\
} while(0)

namespace im = ImGui;

#pragma region default shader
const char* vtxShader3D = R"(
#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
uniform mat4 model;
uniform mat4 VP;
out vec3 FragPos;
out vec3 Normal;
out vec2 UV;
void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    UV=aUV;
    gl_Position = VP * vec4(FragPos, 1.0);
})";

const char* frgShader3D = R"(
#version 430 core
in vec3 FragPos;
in vec3 Normal;
in vec2 UV;
out vec4 FragColor;
uniform vec3 lightDir;
uniform vec3 viewPos;
void main()
{
    // 光照计算
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0);
    
    float diff = max(dot(Normal, normalize(lightDir)), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);
    
    /*float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0, 1.0, 1.0);*/
    
    vec3 result = (ambient*ambientStrength + diffuse*(1-ambientStrength));
    FragColor = vec4(result, 1.0);
})";

const char* vtxShader2D=R"(
#version 430 core

layout(location = 0) in vec3 v2VertexPos2D;

void main()
{
    gl_Position = vec4(v2VertexPos2D, 1.0f);
}
)";

const char* frgShader2D=R"(
#version 430 core

out vec3 v3ColourOut;

void main()
{
    //v3ColourOut = vec3(1.0f, 1.0f, 1.0f);
    v3ColourOut = vec3(1.0f, 1.0f, 0.0f);
}
)";

Shader defaultShader;
#pragma endregion

Shader::Shader() :shaderProgram(NULL)
{}
void Shader::initFromMemory(const char* vtxShader, const char* frgShader) {
    shaderProgram = compileShaders(compileShader(vtxShader, GL_VERTEX_SHADER), compileShader(frgShader, GL_FRAGMENT_SHADER));
}
void Shader::initFromFile(const char* vtxShader, const char* frgShader) {
    shaderProgram = compileShaders(compileShaderFromFile(vtxShader, GL_VERTEX_SHADER), compileShaderFromFile(frgShader, GL_FRAGMENT_SHADER));
}

GLuint Shader::compileShaderFromFile(const char* filename, GLenum type) {
    // 打开文件
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        CERR << "Error opening file [" << filename << "]\n";
        return NULL;
    }

    // 获取文件大小
    fseek(file, 0, SEEK_END);   // 将文件指针移动到文件末尾
    long fileSize = ftell(file); // 获取文件大小
    fseek(file, 0, SEEK_SET);   // 将文件指针移动回文件开头

    // 创建一个与文件大小相等的缓冲区
    char *buffer = (char *)malloc(fileSize+1);
    if (buffer == NULL) {
        CERR << "Memory allocation failed\n";
        fclose(file);
        free(buffer);
        return NULL;
    }

    // 读取文件内容到缓冲区
    size_t bytesRead = fread(buffer, 1, fileSize, file);
    if (bytesRead != fileSize) {
        CERR << "Error reading file\n";
        free(buffer);
        fclose(file);
        return NULL;
    }

    // 关闭文件
    fclose(file);
    buffer[fileSize] = 0;

    GLuint ret = compileShader(buffer, type);
    free(buffer);
    return ret;
}

GLuint Shader::compileShader(const char* str, GLenum type) {
	GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &str, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        CERR << "Shader compilation failed: " << infoLog << '\n';
    }

    return shader;
}

GLuint Shader::compileShaders(GLuint vertexShader, GLuint fragmentShader) {
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        CERR << "Program linking failed: " << infoLog << '\n';
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

GLuint Shader::getShader() const {
    if (shaderProgram == NULL)
        CERR << "Getting an uninitialized shader\n";
    return shaderProgram;
}

const Shader& Shader::GetDefaultShader() {
    if(defaultShader.shaderProgram==NULL)
		defaultShader.initFromMemory(vtxShader3D, frgShader3D);
    return defaultShader;
}

void Shader::UpdateShaderVariables(const Renderer& renderer) const {
    glUseProgram(shaderProgram);
    //view projection matrix
    GLuint VPloc=glGetUniformLocation(shaderProgram, "VP");
    glUniformMatrix4fv(VPloc, 1, GL_FALSE, &renderer.VP[0].x);
    //view position
    GLuint viewPos = glGetUniformLocation(shaderProgram, "viewPos");
    glUniform3fv(viewPos, 1, &renderer.camera.transform.position.x);
    //light dir
	GLint lightLoc = glGetUniformLocation(shaderProgram, "lightDir");
	glUniform3fv(lightLoc, 1, &renderer.lightDir.x);
}
void Shader_BlinnPhong::UpdateShaderVariables(const Renderer& renderer) const{
    Super::UpdateShaderVariables(renderer);
    InitLoc(roughnessLoc, "fRoughness");
    InitLoc(diffuseLoc, "v3DiffuseColour");
    InitLoc(specularLoc, "v3SpecularColour");
    glUniform1f(roughnessLoc, roughness);
    glUniform3fv(diffuseLoc, 1, &diffuseColor.x);
    glUniform3fv(specularLoc, 1, &specularColor.x);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex.texID);
}
void Shader_BlinnPhong::Load() {
    initFromFile("Resources\\Shader\\BP.vert", "Resources\\Shader\\BP.frag");
    roughness = 2.f;
    diffuseColor = glm::vec3(1, 1, 1);
    specularColor = glm::vec3(.5, .5, .5);
    tex.LoadFromFile("Resources\\Texture\\tex.png");
    tex.GLGenTexture();
    tex.UploadTexture(tex.texID);
}
void Shader_BlinnPhong::OnGui(){
    if (im::CollapsingHeader("Shader_BlinnPhong")) {
        im::DragFloat("roughness", &roughness);
        im::ColorEdit3("diffuse", &diffuseColor.x);
        im::ColorEdit3("specular", &specularColor.x);
    }
}
#pragma warning(pop)
