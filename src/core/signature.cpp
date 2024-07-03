#include "src/core/include/signature.hpp"

namespace vecs
{

bool Signature::operator == (const Signature& sig)
{
  return id == sig.id;
}

const unsigned long& Signature::value() const
{
  return id;
}

void Signature::reset()
{
  types.clear();
  id = 0;
}

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

void Signature::hash()
{
  if (types.empty())
  {
    id = 0;
    return;
  }
  
  std::string str = "";

  for (const auto& type : types)
    str += type;

  id = std::hash<std::string>{}(str);
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