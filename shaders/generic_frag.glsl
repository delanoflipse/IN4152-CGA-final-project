#version 430

// Global variables for lighting calculations
//layout(location = 1) uniform vec3 viewPos;

layout(location = 1) uniform vec3 cameraPosition;

// MATERIAL
layout(location = 2) uniform vec3 ks;
layout(location = 3) uniform float shininess;

// LIGHTS
layout(location = 4) uniform int directionalLights;
uniform vec3 directionalLightDirections[8];
uniform vec4 directionalLightColors[8];


// Output for on-screen color
layout(location = 0) out vec4 outColor;

// Interpolated output data from vertex shader
in vec3 fragPos; // World-space position
in vec3 fragNormal; // World-space normal

void main()
{
  vec4 colorSum = vec4(0);
  int directionLightCount = min(8, directionalLights);
  float sumFactor = 1 / float(directionLightCount);

  for (int i = 0; i < directionLightCount; i++) {
    vec3 lightDirection = directionalLightDirections[i];
    vec4 lightColor = directionalLightColors[i];

    float lambertian = max(dot(lightDirection, fragNormal), 0.0);
  
    vec3 fragToCam = normalize(cameraPosition - fragPos);
    vec3 reflecLight = reflect(lightDirection, fragNormal);
    vec3 hVec = 0.5 * (reflecLight + fragToCam);

    // vec3 col = ks * lightColor.xyz * pow(dot(hVec, fragNormal), shininess);
    vec3 diffuse = lambertian * lightColor.xyz;
    vec4 colorConribution = vec4(diffuse, 1.0);

    // float factor = dot(hVec, fragNormal);
    // vec4 colorConribution = vec4(vec3(factor), 1.0);
    colorSum += colorConribution;
  }

  // Output the normal as color
  outColor = sumFactor * colorSum;
  // outColor = directionalLightColors[0];
  // outColor = vec4(1.0);
}
