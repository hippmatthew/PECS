/*
*   PECS - objects.hpp
*   Author:     Matthew Hipp
*   Created:    11/15/23
*   Updated:    11/15/23
*/

#ifndef pecs_objects_hpp
#define pecs_objects_hpp

#include "core.hpp"

namespace pecs
{

class Object;
class Triangle;

class Pipeline;
class GraphicsPipeline;

class Object
{
    public:
        Object(const ShaderPaths& sp) : shaderPaths(sp) {}
        
        virtual ~Object() = 0;
        
        virtual void destroy(const Device * device) = 0;
    
    public:
        Pipeline * graphicsPipeline = nullptr;
        Pipeline * computePipeline = nullptr;
        int pipelineTypes = 0x10;

        const ShaderPaths shaderPaths;
    protected:
        // each renderable object needs a pipline, descriptor set, and a buffer
        // vk::DescriptorSet descriptorSet = VK_NULL_HANDLE;
        // vk::Buffer buffer = VK_NULL_HANDLE;
};

class Triangle : public Object
{
    public:
        Triangle(const ShaderPaths& sp) : Object(sp) {}

        ~Triangle() { delete graphicsPipeline; }
    
        virtual void destroy(const Device * device);
};

class Pipeline
{
    public:
        virtual ~Pipeline() = 0;

        void destroyPipeline(const Device * device)
        {
            device->logicalDevice.destroyPipeline(pipeline);
            device->logicalDevice.destroyPipelineLayout(pipelineLayout);
        }

    protected:
        static std::vector<char> readShader(std::string path);
        vk::ShaderModule createShaderModule(const Device * device, const std::vector<char>& shader);

    protected:
       vk::Pipeline pipeline;
       vk::PipelineLayout pipelineLayout;
};

class GraphicsPipeline : public Pipeline
{  
    public:
        GraphicsPipeline(const Device * device, const SwapchainImageDetails& swapchainImageDetails, const ShaderPaths& shaderPaths);
        GraphicsPipeline(const GraphicsPipeline&) = delete;
        GraphicsPipeline(GraphicsPipeline&&) = delete;

        ~GraphicsPipeline() = default;

        GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
        GraphicsPipeline& operator=(GraphicsPipeline&&) = delete;
};

}

#endif /* pecs_objects_hpp */