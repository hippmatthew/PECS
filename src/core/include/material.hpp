#ifndef vecs_core_material_hpp
#define vecs_core_material_hpp

#include <fstream>
#include <string>
#include <utility>
#include <vector>

namespace vecs
{

enum Shader
{
  Vertex,
  Tesselation1,
  Tesselation2,
  Geometry,
  Fragment,
  sCompute
};

class Material
{ 
  public:
    class MaterialBuilder
    {
      public:
        MaterialBuilder(std::string);
        MaterialBuilder(const MaterialBuilder&) = delete;
        MaterialBuilder(MaterialBuilder&&) = delete;

        ~MaterialBuilder();

        MaterialBuilder& operator = (const MaterialBuilder&) = delete;
        MaterialBuilder& operator = (MaterialBuilder&&) = delete;

        MaterialBuilder& vertex_path(std::string);
        MaterialBuilder& tesselation1_path(std::string);
        MaterialBuilder& tesselation2_path(std::string);
        MaterialBuilder& geometry_path(std::string);
        MaterialBuilder& fragment_path(std::string);
        MaterialBuilder& compute_path(std::string);
        Material& build() const;

      private:
        Material * material = nullptr;
    };

  public:
    Material() = default;
    Material(const Material&) = default;
    Material(Material&&) = default;

    ~Material() = default;

    Material& operator = (const Material&) = default;
    Material& operator = (Material&&) = default;

    static MaterialBuilder Builder(std::string);

  private:
    std::vector<Shader> shaders() const;
    std::vector<char> binary(Shader) const;
  
  private:
    std::string tag;
    std::optional<std::string> vertex;
    std::optional<std::string> tesselation1;
    std::optional<std::string> tesselation2;
    std::optional<std::string> geometry;
    std::optional<std::string> fragment;
    std::optional<std::string> compute;

  friend class MaterialBuilder;
  friend class Renderer;
};

};

#endif // vecs_core_material_hpp