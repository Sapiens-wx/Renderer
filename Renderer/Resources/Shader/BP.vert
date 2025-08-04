#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
uniform mat4 model;
uniform mat4 VP;
uniform vec3 viewPos;
uniform vec3 lightDir;

out vec3 FragPos;
out vec3 Normal;
out vec2 UV;
out vec3 viewDir;


void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    UV=aUV;
    viewDir=normalize(viewPos-FragPos);

    gl_Position = VP * vec4(FragPos, 1.0);
}