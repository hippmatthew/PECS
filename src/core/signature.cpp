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

} // namespace vecs