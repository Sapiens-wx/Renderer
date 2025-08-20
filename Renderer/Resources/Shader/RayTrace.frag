#version 430 core
#define RCPPI 0.31830988618379067f
#define RCP4PI 0.07957747154594766788444188f
#define PI 3.141592653589793238f

layout(binding=0) uniform sampler2D tex;
uniform vec3 viewPos;
uniform vec3 lightDir;
uniform vec3 leftTop;
uniform vec3 leftBot;
uniform vec3 rightTop;
uniform vec3 rightBot;

//loop parameters
float stepSize=0.05f;
float phaseK=-.2f;
float scattering=.5f;

in vec3 FragPos;
in vec3 Normal;
in vec2 UV;

out vec4 FragColor;

float RaySphereIntersectionLength(vec3 origin, vec3 rayDir, vec3 center, float radius, out vec3 firstIntersect)
{
    // 保证方向单位化
    rayDir = normalize(rayDir);

    // 光线起点到球心的向量
    vec3 oc = origin - center;

    // 二次方程的系数
    float b = 2.0 * dot(rayDir, oc);
    float c = dot(oc, oc) - radius * radius;

    // 判别式
    float discriminant = b * b - 4.0 * c;

    if (discriminant < 0.0)
        return 0.0; // 没有交点

    // 两个交点的参数 t
    float sqrtDisc = sqrt(discriminant);
    float t1 = (-b - sqrtDisc) * 0.5;
    float t2 = (-b + sqrtDisc) * 0.5;

    // 如果两个交点都在光线背后
    if (t2 < 0.0)
        return 0.0;

    // 如果射线从内部开始，相交长度是 t2
    if (t1 < 0.0)
        return t2;

    // 正常情况，相交长度是 (t2 - t1)
    return t2 - t1;
}

void main()
{
    //screen staff
    vec3 screenRight=rightTop-leftTop;
    vec3 screenUp=rightTop-rightBot;
    vec3 screenPos=leftBot+UV.x*screenRight+UV.y*screenUp;
    vec3 dir=normalize(screenPos-viewPos);
    //ray
    vec3 firstIntersect;
    float radius=1;
    vec3 center=vec3(0,0,0);
    float intersect=RaySphereIntersectionLength(viewPos, dir, center, radius, firstIntersect)/(2*radius);
    //sample the texture
    vec3 color=texture(tex, UV).xyz;
    //begin the loop
    //int numSteps=int(max(intersect/stepSize, 8.f*step(0.00001, intersect)));
    //float usedStepSize=intersect/numSteps;
    int numSteps=10;
    float usedStepSize=radius*2/numSteps;
    numSteps=int(min(numSteps, intersect/usedStepSize));
    vec3 curPos=firstIntersect;
    for(int i=0;i<numSteps;++i){
		// Calculate phase function
		float fPhase = RCP4PI * (1.0f - (phaseK * phaseK));
		float fDotLTCRD = dot(lightDir, dir);
		float fPhaseDenom = 1.0f - (phaseK * fDotLTCRD);
		fPhase /= fPhaseDenom * fPhaseDenom;
	 
		// Calculate total in-scattering
        float radiance=1-RaySphereIntersectionLength(curPos, lightDir, center, radius, firstIntersect)/(2*radius);
        color=mix(color, vec3(radiance, radiance, radiance), fPhase);

        // step
        curPos+=usedStepSize*dir;
    }
    //color
    FragColor= vec4(color, 1);
}
