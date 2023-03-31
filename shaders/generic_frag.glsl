#version 430

#define SHADOW_Z_OFFSET 0.005

//  ---- SCENE SPECIFIC ----
// CAMERA
uniform vec3 cameraPosition;
uniform mat4 transformation;

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
in vec3 origFragPos; // World-space position
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
    float specularReflection = max(0, dot(halfway, fragNormal));
    float specularIntensity = shininess > 0 ? pow(specularReflection, shininess) : 0;
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

  vec4 fragPos4 = vec4(fragPos, 1.0);
  vec4 origFragPos4 = vec4(origFragPos, 1.0);

  for (int i = 0; i < directionLightCount; i++) {
    vec3 lightDirection = directionalLightDirections[i];
    vec4 lightColor = directionalLightColors[i];

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

    float inShadow = 0;
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
    float shadowFactor = (1 - 0.8 * inShadow);
    // float shadowFactor = (1 - 0.95 * inShadow);

    vec4 colorContribution = getLightColorContribition(lightDirection, lightColor, cameraDirection, fragDiffuseColor, fragSpecularColor);

    colorSum += vec4(shadowFactor * colorContribution.xyz, colorContribution.w);
  }

  // Output the normal as color
  vec4 finalColor = colorSum / float(directionLightCount);
  outColor = finalColor;
}
