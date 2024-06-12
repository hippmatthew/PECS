#ifndef pecs_core_singular_hpp
#define pecs_core_singular_hpp

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

#endif // pecs_core_singular_hpp