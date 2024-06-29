#ifndef vecs_core_entities_hpp
#define vecs_core_entities_hpp

#include "src/core/include/settings.hpp"
#include "src/core/include/signature.hpp"

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
    
    void newEntity();
    void removeEntity(unsigned long);

    template <typename... Tps>
    std::set<unsigned long> retrieve() const;

    template <typename... Tps>
    void addComponents(unsigned long);

    template <typename... Tps>
    void removeComponents(unsigned long);

  private:
    void resize();
    void sort(unsigned long);
  
  private:
    std::vector<Signature> signatures;
    std::set<unsigned long> e_ids;
    std::stack<unsigned long> nextID;
};

} // namespace vecs

#endif // vecs_core_entities_hpp