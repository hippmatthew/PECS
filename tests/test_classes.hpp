#ifndef vecs_tests_test_classes_hpp
#define vecs_tests_test_classes_hpp

#include "src/core/include/entities.hpp"
#include "src/core/include/signature.hpp"

namespace TEST
{

class EntityManager : public vecs::EntityManager
{
  public:
    const unsigned long stack_top() const { return nextID.top(); }
};

class Signature : public vecs::Signature
{
  public:
    const std::bitset<VECS_LIMIT>& bitset() const { return bits; }
};

} // namespace TEST

#endif // vecs_tests_test_classes_hpp