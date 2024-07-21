#include "vecs/vecs.hpp"

#define CATCH_CONFIG_RUNNER
#include <catch2/catch_all.hpp>

#include <iostream>

int main()
{
  int result = Catch::Session().run();

  vecs::Engine engine;
  engine.load();
  engine.run();

  return result;
}