#include "tests/test_classes.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "system_add", "[systems][systemadd]" )
{
  struct TestType1
  {
    int a = 1;
  };

  struct TestType2
  {
    int a = 1;
  };

  class TestSystem : public vecs::System
  {
    public:
      void update(const std::shared_ptr<vecs::ComponentManager>&, std::set<unsigned long>) override {}
  };

  TestSystem system;
  TEST::Signature signature;

  system.addComponents<TestType1, TestType2>();
  signature.set<TestType1, TestType2>();

  CHECK( system.signature() == signature );
}

TEST_CASE( "system_remove", "[systems][systemremove]" )
{
  struct TestType1
  {
    int a = 1;
  };

  struct TestType2
  {
    int a = 1;
  };

  TEST::System system;
  TEST::Signature signature;

  system.addComponents<TestType1, TestType2>();
  system.removeComponents<TestType1>();
  
  signature.set<TestType1, TestType2>();

  CHECK( system.signature() != signature );
}

TEST_CASE( "system_update", "[systems][systemupdate]" )
{
  TEST::System system;
  TEST::Signature signature;

  system.update(nullptr, {});

  CHECK ( system.updated );
}

TEST_CASE( "emplace", "[systems][emplace]" )
{  
  TEST::SystemManager manager;

  manager.emplace<TEST::System>();

  CHECK( manager.has_system<TEST::System>() );
}

TEST_CASE( "erase", "[systems][erase]" )
{
  TEST::SystemManager manager;

  manager.emplace<TEST::System>();
  manager.erase<TEST::System>();

  CHECK( !manager.has_system<TEST::System>() );
}

TEST_CASE( "system", "[systems][system]" )
{
  TEST::SystemManager manager;

  manager.emplace<TEST::System>();
  auto system_opt = manager.system<TEST::System>();
  
  REQUIRE( system_opt.has_value() );
  REQUIRE( system_opt.value() != nullptr );

  auto system = system_opt.value();
  system->update(nullptr, {});

  CHECK( system->updated );
}

TEST_CASE( "add_components", "[systems][addcomponents]" )
{
  struct TestType1
  {
    int a = 1;
  };

  struct TestType2
  {
    int b = 1;
  };

  TEST::SystemManager manager;
  TEST::Signature signature;

  manager.emplace<TEST::System>();
  manager.add_components<TEST::System, TestType1, TestType2>();
  auto system = manager.system<TEST::System>().value();

  signature.set<TestType1, TestType2>();
  
  CHECK( system->signature() == signature );
}

TEST_CASE( "remove_components", "[systems][remove_components]" )
{
  struct TestType1
  {
    int a = 1;
  };

  struct TestType2
  {
    int b = 1;
  };

  TEST::SystemManager manager;
  TEST::Signature signature;

  manager.emplace<TEST::System>();
  manager.add_components<TEST::System, TestType1, TestType2>();
  manager.remove_components<TEST::System, TestType1>();
  auto system = manager.system<TEST::System>().value();

  signature.set<TestType1, TestType2>();

  CHECK( system->signature() != signature );
}