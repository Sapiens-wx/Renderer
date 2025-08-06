#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
uniform mat4 model;
uniform mat4 VP;
uniform vec3 viewPos;

out vec3 Color;
void main()
{
    vec3 FragPos = vec3(model * vec4(aPos, 1.0));
    //viewDir=normalize(viewPos-FragPos);

    gl_Position = VP * vec4(FragPos, 1.0);
    Color=aColor;
}
