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

#include "../../core/include/device.hpp"
#include "../../core/include/engine.hpp"

namespace pecs
{

struct ShaderPaths
{
    std::string vertex = "";
    std::string fragment = "";
    std::string compute = "";
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

#endif /* pecs_pipeline_hpp */