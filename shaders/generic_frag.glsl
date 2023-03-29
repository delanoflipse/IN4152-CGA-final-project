#version 430

//  ---- SCENE SPECIFIC ----
// CAMERA
uniform vec3 cameraPosition;

// LIGHTS
uniform int directionalLights;
uniform vec3 directionalLightDirections[2];
uniform vec4 directionalLightColors[2];
uniform mat4 directionalLightMVPs[2];
uniform sampler2D directionalLightShadowMaps[2];

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
  vec4 fragDiffuseColor = vec4(diffuseColor, 1);
  vec4 fragSpecularColor = vec4(specularColor, 1);


  if (useTexture == 1) {
    fragDiffuseColor = texture(uvTexture, fragUv);
    // fragSpecularColor = fragDiffuseColor;
  }

  vec4 colorSum = vec4(0);
  int directionLightCount = min(2, directionalLights);

  for (int i = 0; i < directionLightCount; i++) {
    vec3 lightDirection = directionalLightDirections[i];
    vec4 lightColor = directionalLightColors[i];

    float lightNormalAngle = dot(lightDirection, fragNormal);

    // if (lightNormalAngle < 0) {
    //   continue;
    // }s

    float lambertian = max(0, lightNormalAngle);
    vec4 diffuse = lambertian * fragDiffuseColor;
  
    vec3 fragToCam = normalize(cameraPosition - fragPos);
    vec3 lightReflected = -reflect(lightDirection, fragNormal);
    // vec3 h = 0.5 * (lightReflected + fragToCam);

    float specularIntensity = max(0, pow(dot(lightReflected, fragToCam), shininess));
    vec4 specular = fragSpecularColor * specularIntensity;

    // vec4 colorContribution = diffuse + vec4(specular.xyz, 0);
    vec4 colorContribution = diffuse;
    // vec4 colorContribution = vec4(vec3(specularIntensity), 1);

    colorSum += colorContribution;
    // colorSum += vec4(colorContribution.xyz, 1);
  }

  // Output the normal as color
  vec4 finalColor = colorSum / float(directionLightCount);
  outColor = vec4(finalColor.xyz, 1);
  // outColor = directionalLightColors[0];
  // outColor = vec4(1.0);
}
