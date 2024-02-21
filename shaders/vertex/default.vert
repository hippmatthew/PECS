#version 460

layout(set = 0, binding = 0) uniform ProjectionData {
  mat4 model;
  mat4 view;
  mat4 projection;
} projection;

layout(location = 0) in vec3 inPosition;

layout (location = 0) out vec3 fragColor;

void main()
{
  gl_Position = projection.projection * projection.view * projection.model * vec4(inPosition, 1.0);
}