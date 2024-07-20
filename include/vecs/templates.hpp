// vecs::vecs_templates_hpp version 0.0.16.0 generated on 07-20-2024 19:09:59 with system Linux

#ifndef vecs_templates_hpp
#define vecs_templates_hpp

namespace vecs
{

template <typename T>
std::optional<T> ComponentArray<T>::at(unsigned long e_id) const
{
  return valid(e_id) ? std::optional<T>(data[indexMap.at(e_id)]) : std::nullopt;
}

template <typename T>
void ComponentArray<T>::emplace(unsigned long e_id, T& e_data)
{
  if (valid(e_id))
  {
    data[indexMap.at(e_id)] = e_data;
    return;
  }

  indexMap.emplace(std::make_pair(e_id, data.size()));
  data.emplace_back(e_data);
}

template <typename T>
void ComponentArray<T>::erase(unsigned long e_id)
{
  if (!valid(e_id)) return;

  data.erase(data.begin() + indexMap.at(e_id));
  indexMap.erase(e_id);
}

template <typename T>
bool ComponentArray<T>::valid(unsigned long e_id) const
{
  return indexMap.find(e_id) != indexMap.end();
}

template <typename... Tps>
void ComponentManager::registerComponents()
{
  ( registerComponent<Tps>(), ... );
}

template <typename... Tps>
void ComponentManager::unregisterComponents()
{
  ( unregisterComponent<Tps>, ... );
}

template <typename... Tps>
void ComponentManager::updateData(unsigned long e_id, Tps&... args)
{
  ( update<Tps>(e_id, args), ... );
}

template <typename... Tps>
void ComponentManager::removeData(unsigned long e_id)
{
  ( remove<Tps>(e_id), ... );
}

template <typename T>
std::optional<T> ComponentManager::retrieve(unsigned long e_id)
{
  return registered<T>() ? array<T>()->at(e_id) : std::nullopt;
}

template <typename T>
bool ComponentManager::registered() const
{
  return componentMap.find(typeid(T).name()) != componentMap.end();
}

template <typename T>
std::shared_ptr<ComponentArray<T>> ComponentManager::array() const
{
  return std::static_pointer_cast<ComponentArray<T>>(componentMap.at(typeid(T).name()));
}

template <typename T>
void ComponentManager::registerComponent()
{
  if (registered<T>()) return;

  componentMap.emplace(std::make_pair(typeid(T).name(), std::make_shared<ComponentArray<T>>()));
}

template <typename T>
void ComponentManager::unregisterComponent()
{
  if (!registered<T>()) return;

  componentMap.erase(typeid(T).name());
}

template <typename T>
void ComponentManager::update(unsigned long e_id, T& e_data)
{
  if (!registered<T>()) return;
  
  array<T>()->emplace(e_id, e_data);
}

template <typename T>
void ComponentManager::remove(unsigned long e_id)
{
  if (!registered<T>()) return;
  
  array<T>()->erase(e_id);
}

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


template <typename T>
unsigned short Settings::component_id()
{
  const char * typeName = typeid(T).name();
  
  if (s_idMap.find(typeName) == s_idMap.end() && nextID != s_maxComponents)
    s_idMap.emplace(std::make_pair(typeName, nextID++));

  return s_idMap.at(typeName);
}

template <typename... Tps>
void Signature::set()
{
  ( add<Tps>(), ... );
}

template <typename... Tps>
void Signature::unset()
{
  ( remove<Tps>(), ... );
}

template <typename T>
void Signature::add()
{
  bits.set(VECS_SETTINGS.component_id<T>());
}

template <typename T>
void Signature::remove()
{
  bits.set(VECS_SETTINGS.component_id<T>(), false);
}


template <typename... Tps>
void System::addComponents()
{
  sys_signature.set<Tps...>();
}

template <typename... Tps>
void System::removeComponents()
{
  sys_signature.unset<Tps...>();
}

template <typename T>
std::optional<std::shared_ptr<T>> SystemManager::system() const
{
  if (!registered<T>()) return std::nullopt;

  return std::optional<std::shared_ptr<T>>(
    std::static_pointer_cast<T>(systemMap.at(typeid(T).name()))
  );
}

template <typename... Tps>
void SystemManager::emplace()
{
  ( add<Tps>(), ... );
}

template <typename... Tps>
void SystemManager::erase()
{
  ( remove<Tps>(), ... );
}

template <typename T, typename... Tps>
void SystemManager::addComponents()
{
  if (!registered<T>()) return;

  systemMap.at(typeid(T).name())->addComponents<Tps...>();
}

template <typename T, typename... Tps>
void SystemManager::removeComponents()
{
  if (!registered<T>()) return;

  systemMap.at(typeid(T).name())->removeComponents<Tps...>();
}

template <typename T>
bool SystemManager::registered() const
{
  return systemMap.find(typeid(T).name()) != systemMap.end();
}

template <typename T>
void SystemManager::add()
{
  if (!std::is_base_of<System, T>::value || registered<T>()) return;

  systemMap.emplace(std::make_pair(typeid(T).name(), std::make_shared<T>()));
};

template <typename T>
void SystemManager::remove()
{
  if (!registered<T>()) return;

  systemMap.erase(typeid(T).name());
}

} // namespace vecs

#endif // vecs_templates_hpp