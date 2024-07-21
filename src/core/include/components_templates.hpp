namespace vecs
{

template <typename T>
const T& ComponentArray<T>::at(unsigned long e_id) const
{
  if (!valid(e_id))
    throw std::runtime_error("error @ ComponentArray<" + std::string(typeid(T).name()) + ">::at() : invalid e_id");
  
  return data[indexMap.at(e_id)];
}

template <typename T>
void ComponentArray<T>::emplace(unsigned long e_id, T e_data)
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
void ComponentManager::register_components()
{
  ( registerComponent<Tps>(), ... );
}

template <typename... Tps>
void ComponentManager::unregister_components()
{
  ( unregisterComponent<Tps>(), ... );
}

template <typename... Tps>
void ComponentManager::update_data(unsigned long e_id, Tps... args)
{
  ( update<Tps>(e_id, args), ... );
}

template <typename... Tps>
void ComponentManager::remove_data(unsigned long e_id)
{
  ( remove<Tps>(e_id), ... );
}

template <typename T>
std::optional<T> ComponentManager::retrieve(unsigned long e_id)
{
  return registered<T>() ? std::optional<T>(array<T>()->at(e_id)) : std::nullopt;
}

template <typename T>
bool ComponentManager::registered() const
{
  return componentMap.find(typeid(T).name()) != componentMap.end();
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

template <typename T>
std::shared_ptr<ComponentArray<T>> ComponentManager::array() const
{
  return std::static_pointer_cast<ComponentArray<T>>(componentMap.at(typeid(T).name()));
}

} // namespace vecs