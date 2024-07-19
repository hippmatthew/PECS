namespace vecs
{

template <typename... Tps>
std::set<unsigned long> EntityManager::retrieve(bool exactMatch) const
{
  Signature signature;
  signature.set<Tps...>();

  std::set<unsigned long> entities;
  
  unsigned long index = 0;
  for (const auto& s : signatures)
  {
    if (exactMatch ? signature & s : s & signature)
      entities.emplace(idMap.at(index++));
  }

  return entities;
}

template <typename... Tps>
void EntityManager::add_components(unsigned long e_id)
{
  if (!valid(e_id)) return;

  signatures[indexMap.at(e_id)].set<Tps...>();
}

template <typename... Tps>
void EntityManager::remove_components(unsigned long e_id)
{
  if (!valid(e_id)) return;

  signatures[indexMap.at(e_id)].unset<Tps...>();
}

} // namespace vecs