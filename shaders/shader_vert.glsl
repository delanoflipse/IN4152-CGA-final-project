#version 450 core

// Model/view/projection matrix
uniform mat4 mvp;
uniform mat4 transformation;
uniform mat3 normalTransformation;

// Per-vertex attributes
layout(location = 0) in vec3 pos; // World-space position
layout(location = 1) in vec3 normal; // World-space normal
layout(location = 2) in vec2 uv; // World-space normal

// Data to pass to fragment shader
out vec3 origFragPos;
out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragUv;

void main() {
    vec4 realPos = transformation * vec4(pos, 1.0);
    vec3 realNormal = normalTransformation * normal;
	// Transform 3D position into on-screen position
    gl_Position = mvp * realPos;

    // Pass position and normal through to fragment shader
    origFragPos = pos;
    fragPos = realPos.xyz / realPos.w;
    // fragNormal = normalize(realNormal.xyz / realNormal.w);
    // fragNormal = normal;
    fragNormal = normalize(realNormal);
    fragUv = uv;
}
