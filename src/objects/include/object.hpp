/*
*   PECS - object.hpp
*   Author:     Matthew Hipp
*   Created:    11/6/23
*   Updated:    11/6/23
*/

#ifndef pecs_object_hpp
#define pecs_object_hpp

#include "pipeline.hpp"

namespace pecs
{

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

}

#endif /* pecs_object_hpp */