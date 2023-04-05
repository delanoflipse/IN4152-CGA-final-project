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

out vData
{
    vec3 position;
    vec3 normal;
    vec2 uv;
} vertex;

void main() {
    vec4 transformedPosition = transformation * vec4(pos, 1.0);
    vec3 transformedNormal = normalTransformation * normal;
    
    vertex.position = transformedPosition.xyz / transformedPosition.w;
    vertex.normal = normalize(transformedNormal);
    vertex.uv = uv;

	// Transform 3D position into on-screen position
    gl_Position = mvp * transformedPosition;
}
