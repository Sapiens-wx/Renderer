#version 430 core
#define RCPPI 0.31830988618379067f
#define PI 3.141592653589793238f

layout(binding=0) uniform sampler2D tex;

in vec3 FragPos;
in vec3 Normal;
in vec2 UV;

out vec4 FragColor;

void main()
{
    FragColor= texture(tex, UV);
}
