#version 430

//  ---- SCENE SPECIFIC ----
// CAMERA
uniform vec3 cameraPosition;
uniform mat4 transformation;

// OPTIONS
uniform int useShadows;
uniform int useLights;

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
uniform int useDiffuseTexture;
uniform sampler2D diffuseTexture;

uniform int useShadowTexture;
uniform sampler2D shadowTexture;

uniform int useOverlayTexture;
uniform sampler2D overlayTexture;


//  ---- INPUT/OUTPUT ----
// Output for on-screen color
layout(location = 0) out vec4 outColor;

// Interpolated output data from vertex shader
in vec3 origFragPos; // World-space position
in vec3 fragPos; // World-space position
in vec3 fragNormal; // World-space normal
in vec2 fragUv; // World-space normal


float getDiffuse(float lightNormalAngle)
{
  return max(0, lightNormalAngle);
}

float getSpecular(vec3 lightDirection, vec3 cameraDirection)
{
  vec3 lightReflected = -reflect(lightDirection, fragNormal);
  vec3 halfway = normalize(lightDirection + cameraDirection);

  // float specularReflection = max(0, dot(lightReflected, cameraDirection));
  float specularReflection = max(0, dot(halfway, fragNormal));
  float specularIntensity = shininess > 0 ? pow(specularReflection, shininess) : 0;

  return specularIntensity;
}

void main()
{
  vec3 cameraDirection = normalize(cameraPosition - fragPos);
  if (dot(cameraDirection, fragNormal) < 0) {
    discard;
  }

  vec4 fragDiffuseColor = vec4(diffuseColor, 1);
  vec4 fragSpecularColor = vec4(specularColor, 1);
  vec4 fragShadowColor = vec4(vec3(0), 1);

  if (useDiffuseTexture == 1) {
    fragDiffuseColor = texture(diffuseTexture, fragUv);
    fragSpecularColor = fragDiffuseColor;
  }

  if (useShadowTexture == 1) {
    fragShadowColor = texture(shadowTexture, fragUv);
  }

  vec4 colorSum = vec4(0);

  vec4 fragPos4 = vec4(fragPos, 1.0);
  vec4 origFragPos4 = vec4(origFragPos, 1.0);

  if (useLights == 0) {
    outColor = fragDiffuseColor;
    return;
  }

  int directionLightCount = min(2, directionalLights);
  for (int i = 0; i < directionLightCount; i++) {
    vec3 lightDirection = directionalLightDirections[i];
    vec4 lightColor = directionalLightColors[i];
    
    float lightNormalAngle = dot(lightDirection, fragNormal);
    float diffuse = getDiffuse(lightNormalAngle);
    float specular = getSpecular(lightDirection, cameraDirection);
    float inShadow = 0;

    if (useShadows == 1) {
      // vec4 fragLightCoord = directionalLightMVPs[i] * fragPos4;
      vec4 fragLightCoord = directionalLightMVPs[i] * transformation *  origFragPos4;

      // Divide by w because fragLightCoord are homogeneous coordinates
      vec3 asLightPosition = fragLightCoord.xyz / fragLightCoord.w;

      // The resulting value is in NDC space (-1 to +1),
      //  we transform them to texture space (0 to 1).
      vec2 shadowMapCoord = asLightPosition.xy * 0.5 + 0.5;

      // Depth of the fragment with respect to the light
      float fragLightDepth = asLightPosition.z * 0.5 + 0.5;
      
      bool outsideShadow = shadowMapCoord.x > 1 || shadowMapCoord.x < 0 || shadowMapCoord.y > 1 || shadowMapCoord.y < 0 || fragLightDepth < -1 || fragLightDepth > 1;

      float bias = max(0.0025 * (1 - dot(fragNormal, lightDirection)), 0.005);
      int samples = 2;
      int totalSamples = (2 * samples + 1) * (2 * samples + 1);
      for (int y = -samples; y <= samples; y++) {
        for (int x = -samples; x <= samples; x++) {
          
          float shadowMapDepth = texture(directionalLightShadowMaps[i], shadowMapCoord).x;
          if (fragLightDepth - bias >= shadowMapDepth) {
            inShadow += 1;
          }
        }
      }

      inShadow /= totalSamples;
    }
    float ambient = 0.05;
    float shadowFactor = (1 - inShadow);
    float lightness = max(ambient, diffuse * shadowFactor);
    float specularness = specular * shadowFactor;

    colorSum += mix(fragShadowColor, fragDiffuseColor, lightness) + specularness * fragSpecularColor * lightColor;
  }

  // Output the normal as color
  vec4 finalColor = colorSum / directionLightCount;
  outColor = finalColor;
}
