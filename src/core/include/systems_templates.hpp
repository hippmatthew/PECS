namespace vecs
{

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
void SystemManager::add_components()
{
  if (!registered<T>()) return;

  systemMap.at(typeid(T).name())->addComponents<Tps...>();
}

template <typename T, typename... Tps>
void SystemManager::remove_components()
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