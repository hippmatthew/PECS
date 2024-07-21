#include "src/core/include/signature.hpp"

namespace vecs
{

Signature Signature::operator & (const Signature& rhs) const
{
  Signature signature;
  signature.bits = bits & rhs.bits;

  return signature;
}

bool Signature::operator == (const Signature& rhs) const
{
  return bits == rhs.bits;
}

void Signature::reset()
{
  bits.reset();
}

} // namespace vecs