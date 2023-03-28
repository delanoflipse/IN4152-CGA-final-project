#version 450 core

#define SHADOW_Z_OFFSET 0.001

// Global variables for lighting calculations.
layout(location = 1) uniform vec3 viewPos;

layout(location = 2) uniform sampler2D texShadow;

layout(location = 3) uniform mat4 lightMVP;
layout(location = 4) uniform vec3 lightPos = vec3(3, 3, 3);

layout(location = 5) uniform sampler2D texLight;
layout(location = 6) uniform float aspectRatio;

// Output for on-screen color.
layout(location = 0) out vec4 outColor;

// Interpolated output data from vertex shader.
in vec3 fragPos; // World-space position
in vec3 fragNormal; // World-space normal

float normalizedScale(float zero, float one, float value) {
    return (value - zero) / (one - zero);
}

void main()
{
    vec4 fragLightCoord = lightMVP * vec4(fragPos, 1.0);

    // Divide by w because fragLightCoord are homogeneous coordinates
    fragLightCoord.xyz /= fragLightCoord.w;

    // The resulting value is in NDC space (-1 to +1),
    //  we transform them to texture space (0 to 1).
    fragLightCoord.xyz = fragLightCoord.xyz * 0.5 + 0.5;

    // Depth of the fragment with respect to the light
    float fragLightDepth = fragLightCoord.z;

    // Shadow map coordinate corresponding to this fragment
    vec2 shadowMapCoord = fragLightCoord.xy;
    bool outsideShadow = shadowMapCoord.x > 1 || shadowMapCoord.x < 0 || shadowMapCoord.y > 1 || shadowMapCoord.y < 0 || fragLightDepth < 0 || fragLightDepth > 1;

    // Shadow map value from the corresponding shadow map position
    float shadowMapDepth = texture(texShadow, shadowMapCoord).x;
    float shadowPosition = shadowMapDepth - fragLightDepth;


    vec3 lightDir = normalize(lightPos - fragPos);
    float lightOutput = max(0, dot(fragNormal, lightDir));

    if (outsideShadow || shadowPosition < -SHADOW_Z_OFFSET) {
    // if (outsideShadow) {
        outColor = vec4(0, 0, 0, 1);
        return;
    }
    // vec4 lightColor = texture(texLight, shadowMapCoord);
    // float lightIntensity = 1;

    vec4 lightColor = vec4(1);
    vec2 ratioAdjustedShadowMap = vec2(aspectRatio * shadowMapCoord.x, shadowMapCoord.y);
    float lightIntensity = 1 - pow(2 * length(vec2(0.5, 0.5) - ratioAdjustedShadowMap), 2.5);

    
    vec4 light = lightIntensity * lightOutput * lightColor;

    outColor = light;
}