#ifndef vecs_core_entities_hpp
#define vecs_core_entities_hpp

#include "src/core/include/settings.hpp"
#include "src/core/include/signature.hpp"

#include <map>
#include <stack>

namespace vecs
{

class EntityManager
{
  public:
    EntityManager();
    EntityManager(const EntityManager&) = delete;
    EntityManager(EntityManager&&) = delete;

    ~EntityManager() = default;

    EntityManager& operator = (const EntityManager&) = delete;
    EntityManager& operator = (EntityManager&&) = delete;

    unsigned long count() const;
    bool valid(unsigned long) const;
    
    void new_entity();
    void remove_entity(unsigned long);

    template <typename... Tps>
    std::set<unsigned long> retrieve() const;

    template <typename... Tps>
    void add_components(unsigned long);

    template <typename... Tps>
    void remove_components(unsigned long);

  protected:
    void resize();
    void sort(unsigned long);
  
  protected:
    std::vector<Signature> signatures;
    std::map<unsigned long, unsigned long> indexMap;
    std::map<unsigned long, unsigned long> idMap;
    std::stack<unsigned long> nextID;
};

} // namespace vecs

#include "src/core/include/entitytemplates.hpp"

#endif // vecs_core_entities_hpp