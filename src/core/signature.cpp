#include "src/core/include/signature.hpp"

namespace vecs
{

bool Signature::operator & (const Signature& rhs) const
{
  return (bits & rhs.bits) == rhs.bits;
}

void Signature::reset()
{
  bits.reset();
}

} // namespace vecs