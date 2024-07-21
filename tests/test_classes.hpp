#ifndef vecs_tests_test_classes_hpp
#define vecs_tests_test_classes_hpp

#include "src/core/include/entities.hpp"
#include "src/core/include/signature.hpp"

#include <bitset>
#include <stack>

namespace TEST
{

class Signature : public vecs::Signature
{
  public:
    const std::bitset<VECS_LIMIT>& bitset() const
    { return bits; }
};

class EntityManager : public vecs::EntityManager
{
  public:
    template <typename T>
    bool has_component(unsigned long e_id) const
    {
      if (!valid(e_id)) return false;
      
      TEST::Signature signature;
      signature.set<T>();

      return (signatures[indexMap.at(e_id)] & signature) == signature;
    }

    const std::stack<unsigned long>& stack() const
    { return nextID; }
    
    const unsigned long stack_top() const
    { return nextID.top(); }

    const unsigned long index_of(unsigned long e_id) const
    { return indexMap.at(e_id); }

    const unsigned long id_of(unsigned long index) const
    { return idMap.at(index); }
};

} // namespace TEST

#endif // vecs_tests_test_classes_hpp