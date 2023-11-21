/*
*   PECS - pipeline.cpp
*   Author:     Matthew Hipp
*   Created:    11/6/23
*   Updated:    11/15/23
*/

#include "include/pipeline.hpp"

namespace pecs
{
    
std::vector<char> Pipeline::readShader(std::string path)
{
    std::ifstream shader(path, std::ios::ate | std::ios::binary);
    if (shader.fail())
        throw std::runtime_error("failed to read shader file");

    unsigned long shaderSize = shader.tellg();
    std::vector<char> buffer(shaderSize);

    shader.seekg(0);
    shader.read(buffer.data(), shaderSize);

    shader.close();
    return buffer;
}

vk::ShaderModule Pipeline::createShaderModule(const Device * device, const std::vector<char>& shader)
{
    vk::ShaderModuleCreateInfo shaderCreateInfo{ .codeSize  = shader.size(),
                                                 .pCode     = reinterpret_cast<const unsigned int *>(shader.data()) };
    
    return device->logicalDevice.createShaderModule(shaderCreateInfo);
}

GraphicsPipeline::GraphicsPipeline(const Device * device, const SwapchainImageDetails& swapchainImageDetails, const ShaderPaths& shaderPaths)
{
   std::vector<vk::PipelineShaderStageCreateInfo> shaderStageCreateInfo;
   std::vector<vk::ShaderModule> shaderModules;

    if (shaderPaths.vertex != "")
    {
        std::vector<char> vertexShader = readShader(shaderPaths.vertex);
        shaderModules.push_back(createShaderModule(device, vertexShader));
        
        shaderStageCreateInfo.push_back(vk::PipelineShaderStageCreateInfo{ .stage   = vk::ShaderStageFlagBits::eVertex,
                                                                           .module  = shaderModules[0],
                                                                           .pName   = "main" });
    }

    if (shaderPaths.fragment != "")
    {
        std::vector<char> fragmentShader = readShader("../../spv/frag/shader.frag.spv");
       shaderModules.push_back(createShaderModule(device, fragmentShader));
        
        shaderStageCreateInfo.push_back(vk::PipelineShaderStageCreateInfo{ .stage     = vk::ShaderStageFlagBits::eFragment,
                                                                           .module    = shaderModules[1],
                                                                           .pName     = "main" });
    }
        
    // Dynamic viewport state info
    std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };

    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo{ .dynamicStateCount = static_cast<unsigned int>(dynamicStates.size()),
                                                               .pDynamicStates    = dynamicStates.data() };

    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo{ .viewportCount = 1,
                                                                 .scissorCount  = 1 };

    // format of the vertex input data. Shader has hard coded vertices so this is irrelevant at the moment. Will revisit when the vartex buffer is made
    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{ .vertexBindingDescriptionCount   = 0,
                                                                       .pVertexBindingDescriptions      = nullptr,
                                                                       .vertexAttributeDescriptionCount = 0,
                                                                       .pVertexAttributeDescriptions    = nullptr };

    // input assembler -- collects data from any specified buffers. Tells the pipeline what primitive to expect the vertex data in
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{ .topology                 = vk::PrimitiveTopology::eTriangleList,
                                                                      .primitiveRestartEnable   = vk::False };
    /*/ 
     *  Tesselation and geometry shader info comes here
     *  Tesselation shader allows you to apply subdivisions to the current mesh which is helpful for things like rendering depth up close
     *  Geometry shader is similar to tesslation shader but it is only performant on intel integrated gpus so its not really used
    /*/
        
    // rasterizer -- fragments the vertex primitives into pixels
    // vk::FrontFace::eClockwise makes the engine render the front of images for clockwise oritented primitives
    // enbaling a depth bias allows the engine to add a constant value dependingon the depth of the vertex. useful for shadows
    vk::PipelineRasterizationStateCreateInfo rasterizerCreateInfo{ .depthClampEnable        = vk::False,
                                                                   .rasterizerDiscardEnable = vk::False,
                                                                   .polygonMode             = vk::PolygonMode::eFill,
                                                                   .lineWidth               = 1.0f,
                                                                   .cullMode                = vk::CullModeFlagBits::eBack,
                                                                   .frontFace               = vk::FrontFace::eClockwise,
                                                                   .depthBiasEnable         = vk::False,
                                                                   .depthBiasConstantFactor = 0.0f,
                                                                   .depthBiasClamp          = 0.0f,
                                                                   .depthBiasSlopeFactor    = 0.0f };
        
    // disabling multisampling for the time being
    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo{ .sampleShadingEnable     = vk::False,
                                                                       .rasterizationSamples    = vk::SampleCountFlagBits::e1,
                                                                       .minSampleShading        = 1.0f,
                                                                       .pSampleMask             = nullptr,
                                                                       .alphaToCoverageEnable   = vk::False,
                                                                       .alphaToOneEnable        = vk::False };

        // create a depth stencil here. Not using one for the time being

        // alpha blending : blend with previous color based on the value of its opacity
    vk::PipelineColorBlendAttachmentState colorBlendAttatchmentState{ .colorWriteMask      = vk::ColorComponentFlagBits::eR |
                                                                                             vk::ColorComponentFlagBits::eG |
                                                                                             vk::ColorComponentFlagBits::eB |
                                                                                             vk::ColorComponentFlagBits::eA,
                                                                      .blendEnable         = vk::True,
                                                                      .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
                                                                      .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
                                                                      .colorBlendOp        = vk::BlendOp::eAdd,
                                                                      .srcAlphaBlendFactor = vk::BlendFactor::eOne,
                                                                      .dstAlphaBlendFactor = vk::BlendFactor::eZero,
                                                                      .alphaBlendOp        = vk::BlendOp::eAdd };

    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{ .logicOpEnable     = vk::False,
                                                                     .logicOp           = vk::LogicOp::eCopy,
                                                                     .attachmentCount   = 1,
                                                                     .pAttachments      = &colorBlendAttatchmentState };
                                                                         
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{ .setLayoutCount          = 0,
                                                           .pSetLayouts             = nullptr,
                                                           .pushConstantRangeCount  = 0,
                                                           .pPushConstantRanges     = nullptr };

    pipelineLayout = device->logicalDevice.createPipelineLayout(pipelineLayoutCreateInfo);
        
    vk::PipelineRenderingCreateInfo dynamicRenderingCreateInfo{ .colorAttachmentCount       = 1,
                                                                .pColorAttachmentFormats    = &swapchainImageDetails.format };

    vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo{ .stageCount          = 2,
                                                               .pStages             = shaderStageCreateInfo.data(),
                                                               .pVertexInputState   = &vertexInputStateCreateInfo,
                                                               .pInputAssemblyState = &inputAssemblyCreateInfo,
                                                               .pViewportState      = &viewportStateCreateInfo,
                                                               .pRasterizationState = &rasterizerCreateInfo,
                                                               .pMultisampleState   = &multisampleStateCreateInfo,
                                                               .pDepthStencilState  = nullptr,
                                                               .pColorBlendState    = &colorBlendStateCreateInfo,
                                                               .pDynamicState       = &dynamicStateCreateInfo,
                                                               .layout              = pipelineLayout,
                                                               .renderPass          = nullptr,
                                                               .pNext               = &dynamicRenderingCreateInfo };

    vk::Result result;
    std::tie(result, pipeline) = device->logicalDevice.createGraphicsPipeline(nullptr, graphicsPipelineCreateInfo);
    switch (result)
    {
        case vk::Result::eSuccess: break;
        case vk::Result::ePipelineCompileRequiredEXT:
            throw std::runtime_error("error creating graphics pipeline");
        default: assert(false);
    }
  
    for (auto shaderModule : shaderModules)
        device->logicalDevice.destroyShaderModule(shaderModule);
}

}