#version 430

#define MAX_LIGHTS 8
#define LIGHT_SAMPLES 6
// #define DEBUG_NORMAL
// #define DEBUG_SHADOW
// #define DEBUG_SHADOW_MAP
// #define DEBUG_SHADOW_MVP
// #define DEBUG_SHADOW_MAP_POS
// #define DEBUG_SHADOW_SPOT

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
uniform float lightShadowPixelSize[MAX_LIGHTS];
uniform float lightDistances[MAX_LIGHTS];
uniform sampler2D lightShadowMaps[MAX_LIGHTS];

//  ---- MESH SPECIFIC ----
// MATERIAL
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float shininess;
uniform float ambient;
uniform float transparency;

// TEXTURES
uniform int useDiffuseTexture;
uniform sampler2D diffuseTexture;

uniform int useShadowTexture;
uniform sampler2D shadowTexture;

uniform int useOverlayTexture;
uniform sampler2D overlayTexture;

uniform float toonUsage;
uniform sampler2D toonTexture;

uniform int useNormalTexture;
uniform sampler2D normalTexture;

uniform int useSpecularTexture;
uniform sampler2D specularTexture;

//  ---- INPUT/OUTPUT ----
// Output for on-screen color
layout(location = 0) out vec4 outColor;

// Interpolated input data
in fData
{
  vec3 position;
  vec3 normal;
  vec2 uv;
  vec3 tangent;
} fragment;

float getDiffuse(vec3 lightDirection, vec3 normal)
{
  return max(0, dot(lightDirection, normal));
}

float getSpecular(vec3 lightDirection, vec3 cameraDirection, vec3 normal)
{
  // vec3 lightReflected = -reflect(lightDirection, normal);
  // float specularReflection = max(0, dot(lightReflected, cameraDirection));

  vec3 halfway = normalize(lightDirection + cameraDirection);
  float specularReflection = max(0, dot(halfway, normal));

  float specularIntensity = shininess > 0 ? pow(specularReflection, shininess) : 0;
  return specularIntensity;
}

vec4 maxVec(vec4 v1, vec4 v2)
{
  return vec4(
    max(v1.x, v2.x),
    max(v1.y, v2.y),
    max(v1.z, v2.z),
    max(v1.w, v2.w)
  );
}

void main()
{

  vec3 toCamera = cameraPosition - fragment.position;
  vec3 cameraDirection = normalize(toCamera);
  // TODO: optional?
  // disabled due to skybox non-inverted normals
  // if (dot(cameraDirection, fragment.normal) < 0) {
  //   discard;
  // }

  vec4 fragDiffuseColor = vec4(diffuseColor, 1);
  vec4 fragSpecularColor = vec4(specularColor, 1);
  vec4 fragShadowColor = vec4(vec3(0), 1);
  vec3 usedNormal = fragment.normal;

  if (useDiffuseTexture == 1) {
    fragDiffuseColor = texture(diffuseTexture, fragment.uv);
  }

  if (useShadowTexture == 1) {
    fragShadowColor = texture(shadowTexture, fragment.uv);
  }

  if (useLights == 0) {
    outColor = fragDiffuseColor;
    return;
  }

  bool useNormalMapping = useNormalTexture == 1;
  if (useNormalMapping) {
    vec3 texNormal = texture(normalTexture, fragment.uv).xyz;
    vec3 mapNormal = normalize(2 * texNormal - 1);

    vec3 N = fragment.normal;
    vec3 T = fragment.tangent;
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = mat3(T, B, N);
    usedNormal = mix(TBN * mapNormal, N, toonUsage);
  }

  if (useSpecularTexture == 1) {
    fragSpecularColor = texture(specularTexture, fragment.uv);
  }

  vec4 colorSum = ambient * fragDiffuseColor;
  vec4 pos4 = vec4(fragment.position, 1.0);

  int numberOfLights = min(MAX_LIGHTS, lightCount);
  float lightNormalize = 1 / float(numberOfLights);

  for (int i = 0; i < numberOfLights; i++) {
    if (lightEnabled[i] == 0) {
      continue;
    }

    int lightType = lightTypes[i];
    vec3 lightDirection = lightDirections[i];
    vec3 lightPosition = lightPosition[i];
    vec3 fromPosToLight = lightPosition - fragment.position;
    float lightFalloffDistance = lightDistances[i];

    // spotlight
    if (lightType == 1) {
      lightDirection = normalize(lightPosition - fragment.position);
    }

    // base values
    vec4 lightColor = lightColors[i];
    
    float diffuse = getDiffuse(lightDirection, usedNormal);
    float specular = getSpecular(lightDirection, cameraDirection, usedNormal);

    float inShadow = 0;

    if (useShadows == 1) {
      // convert to shadow space
      vec4 fragLightCoord = lightMVPs[i] *  pos4;

      // Divide by w because fragLightCoord are homogeneous coordinates
      vec3 asLightPosition = fragLightCoord.xyz / fragLightCoord.w;

      // The resulting value is in NDC space (-1 to +1),
      //  we transform them to texture space (0 to 1).
      vec2 shadowMapCoord = asLightPosition.xy * 0.5 + 0.5;
      #ifdef DEBUG_SHADOW_MAP_POS
      outColor = vec4(shadowMapCoord.x, shadowMapCoord.y, 0, 1);
      return;
      #endif

      // float fragLightDepth = asLightPosition.z;
      float fragLightDepth = asLightPosition.z * 0.5 + 0.5;

      bool insideShadowMap = fragLightDepth >= -1 && fragLightDepth <= 1;
      
      if (insideShadowMap) {
        float bias = 0;
        if (lightType == 1) {
          bias = max(0.00025 * (1 - dot(fragment.normal, lightDirection)), 0.0005);
        } else {
          bias = max(0.00025 * (1 - dot(fragment.normal, lightDirection)), 0.0005);
        }

        int samples = LIGHT_SAMPLES;
        int samplesPerAxis = (2 * samples + 1);
        int totalSamples = samplesPerAxis * samplesPerAxis;
        float pixelSize = lightShadowPixelSize[i];
        

        for (int y = -samples; y <= samples; y++) {
          for (int x = -samples; x <= samples; x++) {
            vec2 shadowPos = shadowMapCoord + vec2(x * pixelSize, y * pixelSize);

            if (shadowPos.x > 1 || shadowPos.x < 0 || shadowPos.y > 1 || shadowPos.y < 0) {
              inShadow += 1;
              continue;
            }

            float shadowMapDepth = texture(lightShadowMaps[i], shadowPos).x;

            #ifdef DEBUG_SHADOW_MAP
            outColor = vec4(vec3(abs(fragLightDepth - shadowMapDepth)), 1);
            // outColor = vec4(vec3(bias), 1);
            return;
            #endif
            
            if (fragLightDepth - bias >= shadowMapDepth) {
              inShadow += 1;
              
              #ifdef DEBUG_SHADOW
              outColor = vec4(1,0,0, 1);
              return;
              #endif
            }
          }
        }

        inShadow /= totalSamples;

        if (lightType == 1) {
          // spotlight
          float coneDropoff = pow(2 * length(vec2(0.5, 0.5) - shadowMapCoord), 2);
          float distanceDropoff = lightFalloffDistance > 0
            ? pow(length(lightPosition - fragment.position) / lightFalloffDistance, 1)
            : 0;
          #ifdef DEBUG_SHADOW_SPOT
          outColor = vec4(vec3(linDepth), 1);
          // outColor = vec4(vec3(bias), 1);
          return;
          #endif

          float intensity = clamp(max(coneDropoff, distanceDropoff), 0, 1);
          inShadow = max(intensity, inShadow);
        }
        
        #ifdef DEBUG_SHADOW_MVP
        outColor = vec4(vec3(fragLightDepth * 0.5 + 0.5), 1);
        return;
        #endif
      } else {
        inShadow = 1;
      }
      
    }

    float shadowFactor = clamp(1.0 - inShadow, 0.0, 1.0);

    #ifdef DEBUG_SHADOW
    outColor = vec4(vec3(shadowFactor), 1);
    return;
    #endif
    

    // TODO: now its x * ambient, which is incorrect
    float lightness = max(0, diffuse * shadowFactor);
    float specularness = max(0, specular * shadowFactor);

    vec4 toonColor = vec4(0);
    if (toonUsage > 0) {
      float z = length(toCamera);
      float dist = clamp(z / 5, 0, 1);
      float brightness = max(lightness, specularness);
      toonColor = texture(toonTexture, vec2(brightness, 1 - dist));
    }

    vec4 genericColor = mix(fragShadowColor, fragDiffuseColor, lightness) * lightColor + specularness * fragSpecularColor * lightColor;
    colorSum += maxVec(vec4(0), mix(genericColor, toonColor, toonUsage));
  }

  // Output the normal as color
  #ifdef DEBUG_NORMAL
  outColor = vec4(usedNormal, 1);
  return;
  #endif

  vec4 finalColor = colorSum * (1 - toonUsage * (1 - lightNormalize));
  // apply gamma correction
  float gamma = 2.2;
  finalColor.rgb = pow(finalColor.rgb, vec3(1.0/gamma));

  finalColor.a = fragDiffuseColor.a;

  outColor = finalColor;
}
