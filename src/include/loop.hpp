/*
 *  PECS - loop.hpp
 *  Author:   Matthew Hipp
 *  Created:  1/26/24
 *  Updated:  1/27/24
 */

#ifndef pecs_loop_hpp
#define pecs_loop_hpp

#include "src/include/singular.hpp"

namespace pecs
{

class Loop : public Singular
{
  public:
    Loop() = default;

    virtual ~Loop() = default;

    virtual void operator () () {}
};

} // namespace pecs

#endif // pecs_loop_hpp