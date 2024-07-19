#ifndef vecs_tests_test_classes_hpp
#define vecs_tests_test_classes_hpp

#include "src/core/include/entities.hpp"

namespace TEST
{

class EntityManager : public vecs::EntityManager
{
  public:
    const unsigned long stack_top() const { return nextID.top(); }
};

} // namespace TEST

#endif // vecs_tests_test_classes_hpp