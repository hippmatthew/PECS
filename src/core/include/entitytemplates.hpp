namespace vecs
{

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
void EntityManager::add_components(unsigned long e_id)
{
  if (!valid(e_id)) return;

  signatures[e_id].set<Tps...>();
}

template <typename... Tps>
void EntityManager::remove_components(unsigned long e_id)
{
  if (!valid(e_id)) return;

  signatures[e_id].unset<Tps...>();
}

} // namespace vecs