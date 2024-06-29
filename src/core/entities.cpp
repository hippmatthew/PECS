#include "src/core/include/entities.hpp"

namespace vecs
{

EntityManager::EntityManager()
{
  signatures.resize(Settings::instance().entity_delta());
  nextID.push(0);
}

unsigned long EntityManager::count() const
{
  return e_ids.size();
}

bool EntityManager::valid(unsigned long e_id) const
{
  return e_ids.find(e_id) != e_ids.end();
}

void EntityManager::newEntity()
{
  if (count() == Settings::instance().max_entities()) return;
  if (count() == signatures.size()) resize();

  e_ids.emplace(nextID.top());
  
  nextID.pop();
  if (nextID.empty()) nextID.push(count());
}

void EntityManager::removeEntity(unsigned long e_id)
{
  if (count() == 0 || !valid(e_id)) return;

  e_ids.erase(e_id);
  
  if (!e_ids.empty())
  {
    sort(e_id);
    return;
  }
    
  signatures.clear();
  signatures.resize(Settings::instance().entity_delta());
    
  while (!nextID.empty())
    nextID.pop();
  nextID.push(0);
}

template <typename... Tps>
std::set<unsigned long> EntityManager::retrieve() const
{
  Signature signature;
  signature.set<Tps...>();

  std::set<unsigned long> entities;
  
  unsigned long e_id = 0, entityCount = 0;
  for (const auto& s : signatures)
  {
    if (!valid(e_id))
    {
      ++e_id;
      continue;
    }
    
    if (s == signature)
      entities.emplace(e_id);

    ++e_id;
    ++entityCount;

    if (entityCount > count()) break;
  }

  return entities;
}

template <typename... Tps>
void EntityManager::addComponents(unsigned long e_id)
{
  if (!valid(e_id)) return;

  signatures[e_id].set<Tps...>();
}

template <typename... Tps>
void EntityManager::removeComponents(unsigned long e_id)
{
  if (!valid(e_id)) return;

  signatures[e_id].unset<Tps...>();
}

void EntityManager::resize()
{
  if (signatures.size() == Settings::instance().max_entities()) return;
  
  auto amount = signatures.size() + Settings::instance().entity_delta();
  if (amount > Settings::instance().max_entities())
    amount = Settings::instance().max_entities();

  signatures.resize(amount);
}

void EntityManager::sort(unsigned long e_id)
{
  if (nextID.empty() || nextID.top() > e_id)
  {
    nextID.push(e_id);
    return;
  }

  auto id = nextID.top();
  nextID.pop();

  sort(e_id);

  nextID.push(id);
}

}