#include <GL/glew.h>
#include <SDL3/SDL_opengl.h>
#include <stdio.h>
#include "Shader.h"
#include "../def.h"
#pragma warning(push)
#pragma warning(disable: 4996)

#pragma region default shader
const char* vtxShader3D = R"(
#version 430 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aNormal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 FragPos;
out vec3 Normal;
void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    //Normal = mat3(transpose(inverse(model))) * aNormal;  
    Normal=vec3(0.,0.,-1.);
    gl_Position = projection * view * vec4(FragPos, 1.0);
})";

const char* frgShader3D = R"(
#version 430 core
in vec3 FragPos;
in vec3 Normal;
out vec4 FragColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
void main()
{
    /*
    // 光照计算
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0);
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);
    
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0, 1.0, 1.0);
    
    vec3 result = (ambient + diffuse + specular);
    gl_FragColor = vec4(result, 1.0);
    */
    FragColor=vec4(1.,0.,0.,1.);
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
        std::cerr << "Program linking failed: " << infoLog << std::endl;
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

GLuint Shader::GetDefaultShader() {
    if(defaultShader.shaderProgram==NULL)
		defaultShader.initFromMemory(vtxShader3D, frgShader3D);
    return defaultShader.getShader();
}
#pragma warning(pop)
