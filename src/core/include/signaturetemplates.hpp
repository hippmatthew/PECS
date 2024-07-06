namespace vecs
{

template <typename... Tps>
void Signature::set()
{
  ( add<Tps>(), ... );
  hash();
}

template <typename... Tps>
void Signature::unset()
{
  ( remove<Tps>(), ... );
  hash();
}

template <typename T>
void Signature::add()
{
  auto type = std::string(typeid(T).name());
  if (types.find(type) != types.end()) return;

  types.emplace(type);
}

template <typename T>
void Signature::remove()
{
  auto type = std::string(typeid(T).name());
  if (types.find(type) == types.end()) return;

  types.erase(type);
}

} // namespace vecs