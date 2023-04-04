#version 430

#define MAX_LIGHTS 8
#define LIGHT_SAMPLES 1
#define DEBUG_SHADOW_MAP
#define PIXEL_SIZE 1 / 1024

//  ---- SCENE SPECIFIC ----
// CAMERA
uniform vec3 cameraPosition;
uniform mat4 transformation;

// OPTIONS
uniform int useShadows;
uniform int useLights;

// LIGHTS
uniform int lightCount;
uniform int lightTypes[MAX_LIGHTS];
uniform int lightEnabled[MAX_LIGHTS];
uniform vec3 lightDirections[MAX_LIGHTS];
uniform vec3 lightPosition[MAX_LIGHTS];
uniform vec4 lightColors[MAX_LIGHTS];
uniform mat4 lightMVPs[MAX_LIGHTS];
uniform sampler2D lightShadowMaps[MAX_LIGHTS];

//  ---- MESH SPECIFIC ----
// MATERIAL
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float shininess;
uniform float ambient;

// TEXTURES
uniform int useDiffuseTexture;
uniform sampler2D diffuseTexture;

uniform int useShadowTexture;
uniform sampler2D shadowTexture;

uniform int useOverlayTexture;
uniform sampler2D overlayTexture;

uniform float toonUsage;
uniform sampler2D toonTexture;


//  ---- INPUT/OUTPUT ----
// Output for on-screen color
layout(location = 0) out vec4 outColor;

// Interpolated output data from vertex shader
in vec3 origFragPos; // World-space position
in vec3 fragPos; // World-space position
in vec3 fragNormal; // World-space normal
in vec2 fragUv; // World-space normal


float getDiffuse(vec3 lightDirection)
{
  return max(0, dot(lightDirection, fragNormal));
}

float getSpecular(vec3 lightDirection, vec3 cameraDirection)
{
  // vec3 lightReflected = -reflect(lightDirection, fragNormal);
  // float specularReflection = max(0, dot(lightReflected, cameraDirection));

  vec3 halfway = normalize(lightDirection + cameraDirection);
  float specularReflection = max(0, dot(halfway, fragNormal));

  float specularIntensity = shininess > 0 ? pow(specularReflection, shininess) : 0;
  return specularIntensity;
}

void main()
{
  vec3 toCamera = cameraPosition - fragPos;
  vec3 cameraDirection = normalize(toCamera);
  // TODO: optional?
  // disabled due to skybox non-inverted normals
  // if (dot(cameraDirection, fragNormal) < 0) {
  //   discard;
  // }

  vec4 fragDiffuseColor = vec4(diffuseColor, 1);
  vec4 fragSpecularColor = vec4(specularColor, 1);
  vec4 fragShadowColor = vec4(vec3(0), 1);

  if (useDiffuseTexture == 1) {
    fragDiffuseColor = texture(diffuseTexture, fragUv);
  }

  if (useShadowTexture == 1) {
    fragShadowColor = texture(shadowTexture, fragUv);
  }

  if (useLights == 0) {
    outColor = fragDiffuseColor;
    return;
  }

  vec4 colorSum = vec4(0);
  vec4 fragPos4 = vec4(fragPos, 1.0);

  int numberOfLights = min(MAX_LIGHTS, lightCount);

  for (int i = 0; i < numberOfLights; i++) {
    if (lightEnabled[i] == 0) {
      continue;
    }

    int lightType = lightTypes[i];
    vec3 lightDirection = lightDirections[i];

    // spotlight
    if (lightType == 1) {
      lightDirection = normalize(lightPosition[i] - fragPos);
    }

    // base values
    vec4 lightColor = lightColors[i];
    
    float diffuse = getDiffuse(lightDirection);
    float specular = getSpecular(lightDirection, cameraDirection);

    float inShadow = 0;

    if (useShadows == 1) {
      // convert to shadow space
      vec4 fragLightCoord = lightMVPs[i] *  fragPos4;

      // Divide by w because fragLightCoord are homogeneous coordinates
      vec3 asLightPosition = fragLightCoord.xyz / fragLightCoord.w;

      // The resulting value is in NDC space (-1 to +1),
      //  we transform them to texture space (0 to 1).
      vec2 shadowMapCoord = asLightPosition.xy * 0.5 + 0.5;

      float fragLightDepth = asLightPosition.z;

      bool insideShadowMap = fragLightDepth >= -1 && fragLightDepth <= 1;
      
      if (insideShadowMap) {
        float bias = 0;
        if (lightType == 1) {
          // bias = 0.001;
          // bias = max(0.00000025 * (1 - dot(fragNormal, lightDirection)), 0.0000005);
        } else {
          bias = max(0.0025 * (1 - dot(fragNormal, lightDirection)), 0.005);
        }

        int samples = LIGHT_SAMPLES;
        int totalSamples = (2 * samples + 1) * (2 * samples + 1);
        for (int y = -samples; y <= samples; y++) {
          for (int x = -samples; x <= samples; x++) {
            vec2 shadowPos = shadowMapCoord + vec2(x * PIXEL_SIZE, y * PIXEL_SIZE);

            if (shadowPos.x > 1 || shadowPos.x < 0 || shadowPos.y > 1 || shadowPos.y < 0) {
              inShadow += 1;
              continue;
            }

            float shadowMapDepth = texture(lightShadowMaps[i], shadowPos).x;
            if (fragLightDepth - bias >= shadowMapDepth) {
              inShadow += 1;
            }
          }
        }

        inShadow /= totalSamples;

        if (lightType == 1) {
          // spotlight
          float coneDropoff = pow(2 * length(vec2(0.5, 0.5) - shadowMapCoord), 1.5);
          // float linearDepth = 1 - 0.9 * (1 / (fragLightDepth * 0.5 + 0.5));
          float linearDepth = 0;
          float intensity = clamp(max(coneDropoff,linearDepth), 0, 1);
          inShadow = max(intensity, inShadow);
        }
        
      } else {
        inShadow = 1;
      }
      
    }

    float shadowFactor = clamp(1 - inShadow, 0.0, 1.0);
    // TODO: now its x * ambient, which is incorrect
    float lightness = max(ambient, diffuse * shadowFactor);
    float specularness = specular * shadowFactor;

    vec4 toonColor = vec4(0);
    if (toonUsage > 0) {
      float z = length(toCamera);
      float dist = z / 10;
      float brightness = max(lightness, specularness);
      toonColor = texture(toonTexture, vec2(brightness, 1 - dist));
    }

    vec4 genericColor = mix(fragShadowColor, fragDiffuseColor, lightness) + specularness * fragSpecularColor * lightColor;
    colorSum += mix(genericColor, toonColor, toonUsage);
  }

  // Output the normal as color
  vec4 finalColor = colorSum;
  outColor = vec4(colorSum.xyz, 1);
}
