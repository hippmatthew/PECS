#include "src/core/include/material.hpp"

namespace vecs
{

Material::MaterialBuilder::MaterialBuilder(std::string tag)
{
  material = new Material;
  material->tag = tag;
}

Material::MaterialBuilder::~MaterialBuilder()
{
  delete material;
}

Material::MaterialBuilder& Material::MaterialBuilder::vertex_path(std::string path)
{
  material->vertex = path;
  return *this;
}

Material::MaterialBuilder& Material::MaterialBuilder::tesselation1_path(std::string path)
{
  material->tesselation1 = path;
  return *this;
}

Material::MaterialBuilder& Material::MaterialBuilder::tesselation2_path(std::string path)
{
  material->tesselation2 = path;
  return *this;
}

Material::MaterialBuilder& Material::MaterialBuilder::geometry_path(std::string path)
{
  material->geometry = path;
  return *this;
}

Material::MaterialBuilder& Material::MaterialBuilder::fragment_path(std::string path)
{
  material->fragment = path;
  return *this;
}

Material::MaterialBuilder& Material::MaterialBuilder::compute_path(std::string path)
{
  material->compute = path;
  return *this;
}

Material& Material::MaterialBuilder::build() const
{
  return *material;
}

Material::MaterialBuilder Material::Builder(std::string materialTag)
{
  return MaterialBuilder(materialTag);
}

std::vector<Shader> Material::shaders() const
{
  std::vector<Shader> paths;
  
  if (vertex.has_value())
    paths.emplace_back(Shader::Vertex);

  if (tesselation1.has_value())
    paths.emplace_back(Shader::Tesselation1);

  if (tesselation2.has_value())
    paths.emplace_back(Shader::Tesselation2);

  if (geometry.has_value())
    paths.emplace_back(Shader::Geometry);

  if (fragment.has_value())
    paths.emplace_back(Shader::Fragment);

  return paths;
}

std::vector<char> Material::binary(Shader type) const
{
  std::string path;
  switch (type)
  {
    case Vertex:
      path = vertex.value();
      break;
    case Tesselation1:
      path = tesselation1.value();
      break;
    case Tesselation2:
      path = tesselation2.value();
      break;
    case Geometry:
      path = geometry.value();
      break;
    case Fragment:
      path = fragment.value();
    case Compute:
      path = compute.value();
  }
  
  std::vector<char> buffer;
  
  std::ifstream file(path, std::ios::ate | std::ios::binary);
  if (file.fail()) return buffer;

  std::size_t size = file.tellg();
  buffer.resize(size);

  file.seekg(0);
  file.read(buffer.data(), size);

  file.close();
  return buffer;
}
  
} // namespace vecs