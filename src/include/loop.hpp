/*
 *  PECS - loop.hpp
 *  Author:   Matthew Hipp
 *  Created:  1/26/24
 *  Updated:  1/26/24
 */

#ifndef pecs_loop_hpp
#define pecs_loop_hpp

#include "src/include/singular.hpp"
#include "src/include/object.hpp"

namespace pecs
{

class Loop : public Singular
{
  public:
    Loop() = default;

    virtual ~Loop();

    virtual void operator () () {}

    std::vector<Object *>& objectData();
    void addObject(const Device&, const ViewportInfo&, const ShaderPaths&);

  protected:
    std::vector<Object *> objects;
};

} // namespace pecs

#endif // pecs_loop_hpp