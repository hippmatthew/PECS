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
        void destroyObject(const Engine& engine);

    protected:
        // each renderable object needs a pipline, descriptor set, and a buffer
        Pipeline *  pipeline = nullptr;
        // vk::DescriptorSet descriptorSet = VK_NULL_HANDLE;
        // vk::Buffer buffer = VK_NULL_HANDLE;
};

class Triangle : public Object
{
    public:
        Triangle(const Engine& engine);
    
        virtual void destroyObject(const Engine& engine);
};

}

#endif /* pecs_object_hpp */