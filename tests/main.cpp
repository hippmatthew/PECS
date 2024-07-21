#include "tests/test_ecs.hpp"

#define CATCH_CONFIG_RUNNER
#include <catch2/catch_all.hpp>

TEST_CASE( "engine", "[engine]" )
{
  TEST::Engine engine;
  
  engine.load();
  engine.run();

  auto values = engine.final_values();

  CHECK( values.first == 11 );
  CHECK( values.second == 3070 );
}

int main()
{
  return Catch::Session().run();
}