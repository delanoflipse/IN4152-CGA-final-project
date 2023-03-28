#version 430

//  ---- SCENE SPECIFIC ----
// CAMERA
uniform vec3 cameraPosition;

// LIGHTS
uniform int directionalLights;
uniform vec3 directionalLightDirections[2];
uniform vec4 directionalLightColors[2];

//  ---- MESH SPECIFIC ----
// MATERIAL
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float shininess;

// TEXTURES
uniform int useTexture;
uniform sampler2D uvTexture;


//  ---- INPUT/OUTPUT ----
// Output for on-screen color
layout(location = 0) out vec4 outColor;

// Interpolated output data from vertex shader
in vec3 fragPos; // World-space position
in vec3 fragNormal; // World-space normal
in vec2 fragUv; // World-space normal

void main()
{
  vec4 fragDiffuse = vec4(diffuseColor, 1);

  if (useTexture == 1) {
    fragDiffuse = texture(uvTexture, fragUv);
  }

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
    vec4 diffuse = lambertian * lightColor * fragDiffuse;
    vec4 colorConribution = diffuse;

    // float factor = dot(hVec, fragNormal);
    // vec4 colorConribution = vec4(vec3(factor), 1.0);
    colorSum += colorConribution;
  }

  // Output the normal as color
  outColor = sumFactor * colorSum;
  // outColor = directionalLightColors[0];
  // outColor = vec4(1.0);
}
