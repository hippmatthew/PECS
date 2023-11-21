#include "include/object.hpp"

namespace pecs
{

void Triangle::destroy(const Device * device)
{
    graphicsPipeline->destroyPipeline(device);
}

}