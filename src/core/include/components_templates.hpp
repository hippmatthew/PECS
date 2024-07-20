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

} // namespace vecs