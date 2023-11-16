#include "include/object.hpp"

namespace pecs
{

void Object::destroyObject(const Engine& engine)
{
    pipeline->destroyPipeline(engine.getDevice());
}

Triangle::Triangle(const Engine& engine)
{
    GraphicsPipeline::ShaderPaths shaderPaths{ .vertex = "~/Documents/GitHub/PECS/spv/triangle.vert.spv",
                                               .fragment    = "~/Documents/GitHub/PECS/spv/triangle.frag.spv" };
    
    pipeline = new GraphicsPipeline(engine.getDevice(), engine.swapchainImageDetails, shaderPaths);
}

}