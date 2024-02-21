#version 460

layout(set = 1, binding = 0) uniform PhysicalProperties {
  vec3 position;
  vec3 momentum;
  float energy;
  float mass;
  float charge;
} properties;

layout(location = 0) out vec4 outColor;

void main()
{    
  float pSquared = properties.momentum.x * properties.momentum.x  +
                    properties.momentum.y * properties.momentum.y +
                    properties.momentum.z * properties.momentum.z;

  float kineticEnergy = pSquared / ( 2 * properties.mass);
  float TRatio = kineticEnergy / properties.energy;
  
  outColor = vec4(TRatio, 0.0f, 1 - TRatio, 1.0f);
}