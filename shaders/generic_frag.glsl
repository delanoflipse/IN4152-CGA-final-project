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

vec4 getLightColorContribition(vec3 lightDirection, vec4 lightColor, vec3 cameraDirection, vec4 diffuseColor, vec4 specularColor)
{
    float lightNormalAngle = dot(lightDirection, fragNormal);

    if (lightNormalAngle < 0) {
      return vec4(vec3(0), 1);
    }

    float lambertian = max(0, lightNormalAngle);
    vec4 diffuse = lambertian * diffuseColor;
  
    vec3 lightReflected = -reflect(lightDirection, fragNormal);
    vec3 halfway = normalize(lightDirection + cameraDirection);

    // float specularReflection = max(0, dot(lightReflected, cameraDirection));
    float specularReflection =  max(0, dot(halfway, fragNormal));
    float specularIntensity = pow(specularReflection, shininess);
    vec4 specular = specularColor * specularIntensity;

    // return diffuse;
    // return specular;
    // return vec4(vec3(specularReflection), 1);
    // return vec4(vec3(specularIntensity), 1);
    // return diffuse + vec4(specular.xyz, 0);

    return diffuse + vec4(specular.xyz, 0);
}

void main()
{
  vec3 cameraDirection = normalize(cameraPosition - fragPos);
  if (dot(cameraDirection, fragNormal) < 0) {
    discard;
  }

  vec4 fragDiffuseColor = vec4(diffuseColor, 1);
  vec4 fragSpecularColor = vec4(specularColor, 1);

  
  // outColor = vec4(fragUv.xy, 0, 1);
  // outColor = vec4(fragNormal, 1);
  // return;

  if (useTexture == 1) {
    fragDiffuseColor = texture(uvTexture, fragUv);
    fragSpecularColor = fragDiffuseColor;
  }

  vec4 colorSum = vec4(0);
  int directionLightCount = min(2, directionalLights);


  for (int i = 0; i < directionLightCount; i++) {
    vec3 lightDirection = directionalLightDirections[i];
    vec4 lightColor = directionalLightColors[i];

    vec4 colorContribution = getLightColorContribition(lightDirection, lightColor, cameraDirection, fragDiffuseColor, fragSpecularColor);

    colorSum += colorContribution;
  }

  // Output the normal as color
  vec4 finalColor = colorSum / float(directionLightCount);
  outColor = finalColor;
}
