#version 460

vec2 positions[3] = vec2[](
  vec2(0.0, 0.75),
  vec2(0.5, 0.1),
  vec2(-0.5, 0.1)
);

vec3 colors[3] = vec3[](
  vec3(0.0, 0.0, 1.0),
  vec3(0.0, 1.0, 0.0),
  vec3(1.0, 0.0, 0.0)
);

layout(location = 0) out vec4 outColor;

void main()
{
  gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
  outColor = vec4(colors[gl_VertexIndex], 1.0);
}