#ifndef vecs_core_signature_hpp
#define vecs_core_signature_hpp

#include "src/core/include/settings.hpp"

#include <bitset>
#include <numeric>
#include <set>

namespace vecs
{

class Signature
{  
  public:
    Signature() = default;
    Signature(const Signature&) = default;
    Signature(Signature&&) = default;

    ~Signature() = default;

    Signature& operator = (const Signature&) = default;
    Signature& operator = (Signature&&) = default;
    bool operator & (const Signature&) const;

    void reset();

    template <typename... Tps>
    void set();

    template <typename... Tps>
    void unset();
  
  private:
    template <typename T>
    void add();

    template <typename T>
    void remove();

  protected:
    std::bitset<VECS_LIMIT> bits;
};

} // namespace vecs

#include "src/core/include/signature_templates.hpp"

#endif // vecs_core_signature_hpp