#include "src/core/include/systems.hpp"

namespace vecs
{

const Signature& System::signature() const
{
  return sys_signature;
}

} // namespace vecs