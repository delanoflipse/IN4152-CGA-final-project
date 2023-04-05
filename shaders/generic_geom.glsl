#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 mvp;
uniform mat4 transformation;
uniform int useNormalTexture;

in vData
{
  vec3 position;
  vec3 normal;
  vec2 uv;
  mat4 model;
} vertex[];

out fData
{
  vec3 position;
  vec3 normal;
  vec2 uv;
  vec3 tangent;
} fragment;    

void main() {
  vec3 p0 = vertex[0].position;
  vec3 p1 = vertex[1].position;
  vec3 p2 = vertex[2].position;

  vec2 uv0 = vertex[0].uv;
  vec2 uv1 = vertex[1].uv;
  vec2 uv2 = vertex[2].uv;

  mat4 model = transformation * mvp;

  // mostly based on https://github.com/VictorGordan/opengl-tutorials/blob/main/YoutubeOpenGL%2027%20-%20Normal%20Maps/default.geom
  vec3 edge01 = p1 - p0;
  vec3 edge02 = p2 - p0;
  vec2 uvEdge01 = uv1 - uv0;
  vec2 uvEdge02 = uv2 - uv0;

  float invDet = 1 / (uvEdge01.x * uvEdge02.y - uvEdge02.x * uvEdge01.y);
  vec3 tangent = invDet * (edge01 * uvEdge02.y - edge02 * uvEdge01.y);
  fragment.tangent = normalize(tangent);
  
  for (int i = 0; i < 3; i++) {
    gl_Position = gl_in[i].gl_Position;

    fragment.normal = vertex[i].normal;
    fragment.uv = vertex[i].uv;
    fragment.position = vertex[i].position;
    EmitVertex();
  }

  EndPrimitive();
}
