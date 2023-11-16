/*
*   PECS - pipeline.hpp
*   Author:     Matthew Hipp
*   Created:    11/6/23
*   Updated:    11/6/23
*/

#ifndef pecs_pipeline_hpp
#define pecs_pipeline_hpp

#include <fstream>
#include <iostream>

#include "device.hpp"
#include "engine.hpp"

namespace pecs
{

class Pipeline
{
    public:
        virtual ~Pipeline() = 0;

        void destroyPipeline(const Device * device)
        {
            device->getLogicalDevice().destroyPipeline(pipeline);
            device->getLogicalDevice().destroyPipelineLayout(pipelineLayout); 
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
        struct ShaderPaths{
            std::string vertex;
            std::string fragment;
        };
        
    public:
        GraphicsPipeline(const Device * device, const SwapchainImageDetails& swapchainImageDetails, const ShaderPaths& shaderPaths);
        GraphicsPipeline(const GraphicsPipeline&) = delete;
        GraphicsPipeline(GraphicsPipeline&&) = delete;

        ~GraphicsPipeline() = default;

        GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
        GraphicsPipeline& operator=(GraphicsPipeline&&) = delete;
};

}

#endif /* pecs_pipeline_hpp */