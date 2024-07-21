#include "tests/test_classes.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "valid", "[entities][valid]" )
{
  TEST::EntityManager manager;

  SECTION( "invalid_entity" )
  {
    CHECK( !manager.valid(0) );
  }

  SECTION( "valid_entity" )
  {
    manager.new_entity();

    CHECK( manager.valid(0) );
  }
}

TEST_CASE( "new_entity", "[entities][new]" )
{
  TEST::EntityManager manager;

  for (unsigned long i = 0; i < 5; ++i)
    manager.new_entity();

  CHECK( manager.count() == 5 );
  CHECK( manager.stack_top() == 5);

  for ( unsigned long i = 0; i < 5; ++i)
    CHECK( manager.valid(i) );
}

TEST_CASE( "remove_entity", "[entities][remove]" )
{
  TEST::EntityManager manager;

  for (unsigned long i = 0; i < 5; ++i)
    manager.new_entity();
  
  SECTION( "removal" )
  {
    manager.remove_entity(2);
    
    CHECK( manager.count() == 4 );
    CHECK( manager.stack_top() == 2 );
    CHECK( !manager.valid(2) );
  }

  SECTION ( "addition_after_removal" )
  {
    manager.remove_entity(2);
    manager.new_entity();
    
    CHECK( manager.count() == 5 );
    CHECK( manager.stack_top() == 5 );
    CHECK( manager.valid(2) );
  }

  SECTION ( "remove_all" )
  {
    for (unsigned long i = 0; i < 5; ++i)
      manager.remove_entity(i);
    
    CHECK( manager.count() == 0 );
    CHECK( manager.stack_top() == 0 );
    
    for(unsigned long i = 0; i < 5; ++i)
      CHECK( !manager.valid(i) );
  }
}

TEST_CASE( "nextID", "[entities][nextid]" )
{
  std::stack<unsigned long> testStack;
  TEST::EntityManager manager;

  for (unsigned long i = 0; i < 5; ++i)
    manager.new_entity();

  manager.remove_entity(1);
  manager.remove_entity(2);
  manager.remove_entity(3);
  
  for (unsigned long i = 5; i > 0; --i)
  {
    if (i == 4) continue;
    testStack.emplace(i);
  }
  
  CHECK( manager.stack() == testStack );
}

TEST_CASE( "add_component", "[entities][addcomponent]" )
{
  struct TestType1
  {
    int a = 0;
  };

  struct TestType2
  {
    int b = 1;
  };

  
  TEST::EntityManager manager;

  manager.new_entity();
  manager.add_components<TestType1, TestType2>(0);

  CHECK( manager.has_component<TestType1>(0) );
  CHECK( manager.has_component<TestType2>(0) );
}

TEST_CASE( "remove_component", "[entities][removecomponent]" )
{
  struct TestType1
  {
    int a = 0;
  };

  struct TestType2
  {
    int b = 1;
  };

  TEST::EntityManager manager;

  manager.new_entity();
  manager.add_components<TestType1, TestType2>(0);
  manager.remove_components<TestType1, TestType2>(0);

  CHECK( !manager.has_component<TestType1>(0) );
  CHECK( !manager.has_component<TestType2>(0) );
}

TEST_CASE( "find_index", "[entities][findindex]" )
{
  TEST::EntityManager manager;

  SECTION( "mapping" )
  {
    manager.new_entity();
    manager.new_entity();

    CHECK( manager.index_of(0) == 0 );
    CHECK( manager.index_of(1) == 1 );
  }

  SECTION( "self_comparison" )
  {
    manager.new_entity();

    CHECK( manager.index_of(0) == manager.index_of(0) );
  } 
}

TEST_CASE( "find_id", "[entities][findid]" )
{
  TEST::EntityManager manager;

  SECTION( "mapping" )
  {
    manager.new_entity();
    manager.new_entity();

    CHECK( manager.id_of(0) == 0 );
    CHECK( manager.id_of(1) == 1);
  }

  SECTION( "self_comparison" )
  {
    manager.new_entity();

    CHECK( manager.id_of(0) == manager.id_of(0) );
  }
}

TEST_CASE( "retrieval", "[entities][retrieval]" )
{
  struct TestType1
  {
    int a = 0;
  };

  struct TestType2
  {
    int b = 1;
  };
  
  TEST::EntityManager manager;

  for (unsigned long i = 0; i < 5; ++i)
    manager.new_entity();

  SECTION( "empty_retrieval" )
  {
    std::set<unsigned long> testSet{ 0, 1, 2, 3, 4 };
    
    auto nonexact = manager.retrieve<>();
    auto exact = manager.retrieve<>(true);
  
    CHECK( nonexact == testSet );
    CHECK( exact == testSet );
  }

  SECTION( "retrieve_all" )
  {    
    for (unsigned long i = 0; i < 3; ++i)
      manager.add_components<TestType1>(i);

    manager.add_components<TestType2>(3);
    manager.add_components<TestType1, TestType2>(4);

    CHECK( manager.retrieve<TestType1>() == std::set<unsigned long>{ 0, 1, 2, 4 } );
    CHECK( manager.retrieve<TestType2>() == std::set<unsigned long>{ 3, 4 } );
    CHECK( manager.retrieve<TestType1, TestType2>() == std::set<unsigned long>{ 4 } );
  }

  SECTION( "retrieve_exact" )
  {
    for (unsigned long i = 0; i < 3; ++i)
      manager.add_components<TestType1, TestType2>(i);
    manager.add_components<TestType1>(3);
    manager.add_components<TestType2>(4);

    CHECK( manager.retrieve<TestType1>(true) == std::set<unsigned long>{3} );
    CHECK( manager.retrieve<TestType2>(true) == std::set<unsigned long>{4} );
    CHECK( manager.retrieve<TestType1, TestType2>(true) == std::set<unsigned long>{0, 1, 2} );
  }
}