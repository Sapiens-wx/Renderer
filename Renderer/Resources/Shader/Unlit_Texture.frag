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
    float near=0.1, far=1000.;
    float depth=texture(tex, UV).x;
    //depth=near*far/(far-(depth*(far-near)));
    //depth=fract(depth);
    FragColor= vec4(depth, depth, depth, 1.);
}
