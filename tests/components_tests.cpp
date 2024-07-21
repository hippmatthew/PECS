#include "tests/test_classes.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "array_emplace", "[components][arrayemplace]" )
{
  struct TestType
  {
    int a = 1;
  };

  TEST::ComponentArray<TestType> componentArray;

  componentArray.emplace(1, { 3 });

  CHECK( componentArray.contains(1) );
}

TEST_CASE( "array_erase", "[components][arrayerase]" )
{
  struct TestType
  {
    int a = 1;
  };

  TEST::ComponentArray<TestType> componentArray;

  componentArray.emplace(1, { 3 });
  componentArray.erase(1);

  CHECK( !componentArray.contains(1) );
}

TEST_CASE( "array_at", "[components][arrayat]" )
{
  struct TestType
  {
    int a = 1;
  };

  TEST::ComponentArray<TestType> componentArray;
  
  componentArray.emplace(1, { 3 });

  CHECK( componentArray.at(1).a == 3 );
}

TEST_CASE( "register", "[components][register]" )
{
  struct TestType1
  {
    int a = 1;
  };

  struct TestType2
  {
    int a = 1;
  };
  
  TEST::ComponentManager manager;

  manager.register_components<TestType1, TestType2>();

  CHECK( manager.registered<TestType1>() );
  CHECK( manager.registered<TestType2>() );
}

TEST_CASE( "unregister", "[components][unregister]" )
{
  struct TestType1
  {
    int a = 1;
  };

  struct TestType2
  {
    int a = 1;
  };
  
  TEST::ComponentManager manager;

  manager.register_components<TestType1, TestType2>();
  manager.unregister_components<TestType1, TestType2>();

  CHECK( !manager.registered<TestType1>() );
  CHECK( !manager.registered<TestType2>() );
}

TEST_CASE( "array", "[components][array]" )
{
  struct TestType
  {
    int a = 1;
  };
  
  TEST::ComponentManager manager;

  manager.register_components<TestType>();

  CHECK( manager.component_array<TestType>() != nullptr );
}

TEST_CASE( "update_data", "[components][updatedata]" )
{
  struct TestType
  {
    int a = 1;
  };

  TEST::ComponentManager manager;

  manager.register_components<TestType>();
  manager.update_data<TestType>(0, { 3 });

  auto array = manager.component_array<TestType>();

  CHECK( array->at(0).a == 3 );
}

TEST_CASE( "remove_data", "[components][removedata]" )
{
  struct TestType
  {
    int a = 1;
  };

  TEST::ComponentManager manager;

  manager.register_components<TestType>();
  manager.update_data<TestType>(0, { 3 });
  manager.remove_data<TestType>(0);

  auto vecs_array = manager.component_array<TestType>();
  TEST::ComponentArray array(*vecs_array);

  CHECK( !array.contains(0) );
}

TEST_CASE( "retrieve", "[components][retrieve]" )
{
  struct TestType
  {
    int a = 1;
  };

  TEST::ComponentManager manager;

  manager.register_components<TestType>();
  manager.update_data<TestType>(0, { 3 });

  auto data = manager.retrieve<TestType>(0);

  REQUIRE( data != std::nullopt );
  CHECK( data.value().a == 3 );
}