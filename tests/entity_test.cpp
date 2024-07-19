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