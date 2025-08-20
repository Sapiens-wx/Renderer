#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
uniform mat4 model;
uniform mat4 VP;
uniform vec3 viewPos;
uniform vec3 leftTop;
uniform vec3 leftBot;
uniform vec3 rightTop;
uniform vec3 rightBot;

out vec3 FragPos;
out vec3 Normal;
out vec2 UV;


void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    UV=aUV;

    gl_Position = VP * vec4(FragPos, 1.0);
    //gl_Position=vec4(UV.x*2.-1.,UV.y*2.-1.,-1., 1.0);
}
