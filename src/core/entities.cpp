#include "src/core/include/entities.hpp"

namespace vecs
{

EntityManager::EntityManager()
{
  nextID.push(0);
}

unsigned long EntityManager::count() const
{
  return signatures.size();
}

bool EntityManager::valid(unsigned long e_id) const
{
  if (indexMap.find(e_id) == indexMap.end()) return false;
  
  unsigned long index = indexMap.at(e_id);

  if (idMap.find(index) == idMap.end()) return false;
  
  return idMap.at(index) == e_id;
}

void EntityManager::new_entity()
{
  if (count() == VECS_SETTINGS.max_entities() || valid(nextID.top())) return;
  
  unsigned long e_id = nextID.top();
  unsigned long index = count();
  
  indexMap.emplace(std::make_pair(e_id, index));
  idMap.emplace(std::make_pair(index, e_id));

  signatures.emplace_back(Signature{});
  
  nextID.pop();
  if (nextID.empty()) nextID.push(count());
}

void EntityManager::remove_entity(unsigned long e_id)
{
  if (count() == 0 || !valid(e_id)) return;

  unsigned long index = indexMap.at(e_id);

  indexMap.erase(e_id);
  idMap.erase(index);
  
  for (unsigned long i = index + 1; i < count(); ++i)
  {
    unsigned long id = idMap.at(i);

    idMap.erase(i);
    indexMap.erase(id);

    indexMap.emplace(std::make_pair(id, i - 1));
    idMap.emplace(std::make_pair(i - 1, id));
  }

  signatures.erase(signatures.begin() + index);

  if (count() == 0)
  {
    while (!nextID.empty())
      nextID.pop();

    nextID.push(0);
    return;
  }

  sort(e_id);
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