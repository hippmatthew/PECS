#ifndef vecs_tests_test_classes_hpp
#define vecs_tests_test_classes_hpp

#include "vecs/vecs.hpp"

#include <bitset>
#include <memory>
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

template <typename T>
class ComponentArray : public vecs::ComponentArray<T>
{
  public:
    ComponentArray() = default;
    ComponentArray(vecs::ComponentArray<T>& array) : vecs::ComponentArray<T>(array) {}
    
    bool contains(unsigned long e_id) const
    { return vecs::ComponentArray<T>::valid(e_id); }
};

class ComponentManager : public vecs::ComponentManager
{
  public:
    template <typename T>
    std::shared_ptr<vecs::ComponentArray<T>> component_array()
    { return array<T>(); }
};

} // namespace TEST

#endif // vecs_tests_test_classes_hpp