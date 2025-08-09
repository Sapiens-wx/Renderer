#version 430 core
#define RCPPI 0.31830988618379067f
#define PI 3.141592653589793238f

layout(binding=0) uniform sampler2D tex;

in vec3 FragPos;
in vec3 Normal;
in vec2 UV;
in vec3 viewDir;
uniform vec3 lightDir;
//Custom Parameters
uniform vec3 v3DiffuseColour;
uniform vec3 v3SpecularColour;
uniform float fRoughness;

out vec4 FragColor;

vec3 blinnPhong(in vec3 v3Normal, in vec3 v3LightDirection, in vec3 v3ViewDirection, in vec3 v3LightIrradiance, in vec3 v3DiffuseColour, in vec3 v3SpecularColour, in float fRoughness)
{
    // Get diffuse component
    vec3 v3Diffuse = v3DiffuseColour*max(dot(v3Normal, v3LightDirection)/2.+.5,.3)*RCPPI;
 
    // Calculate half vector
    vec3 v3HalfVector = normalize(v3ViewDirection + v3LightDirection);
 
    // Calculate specular component
    vec3 v3Specular = pow(max(dot(v3Normal, v3HalfVector), 0.0f), fRoughness) * (fRoughness+8.)/(8.*PI) * v3SpecularColour;

    // Combine diffuse and specular
    float diffuseStr=3.;
    vec3 v3RetColour = v3Diffuse*diffuseStr + v3Specular;
 
    // Combine with incoming light value
    v3RetColour *= v3LightIrradiance;
    return v3RetColour;
}
void main()
{
    vec3 texColor=texture(tex, UV).xyz;
    FragColor= vec4(blinnPhong(Normal, lightDir, viewDir, texColor, v3DiffuseColour, v3SpecularColour, fRoughness),.5);
}