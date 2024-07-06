#ifndef vecs_core_signature_hpp
#define vecs_core_signature_hpp

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
    bool operator == (const Signature&);
    
    const unsigned long& value() const;
    
    void reset();

    template <typename... Tps>
    void set();

    template <typename... Tps>
    void unset();
  
  private:
    void hash();
    
    template <typename T>
    void add();

    template <typename T>
    void remove();

  private:
    std::set<std::string> types;
    unsigned long id = 0;
};

} // namespace vecs

#include "src/core/include/signaturetemplates.hpp"

#endif // vecs_core_signature_hpp