/*
 *  PECS - singular.hpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  1/21/24
 */

#ifndef pecs_singular_hpp
#define pecs_singular_hpp

namespace pecs
{

class Singular
{
  public:
    Singular() = default;
    Singular(const Singular&) = delete;
    Singular(Singular&&) = delete;

    virtual ~Singular() = default;

    Singular& operator = (const Singular&) = delete;
    Singular& operator = (Singular&&) = delete;
};

} // namespace pecs

#endif // pecs_singular_hpp