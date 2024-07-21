#include "tests/test_classes.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "initialize_empty", "[signature][initialization]" )
{
  TEST::Signature signature;
  
  std::bitset<VECS_LIMIT> testBits;
  testBits.set();

  CHECK( (signature.bitset() & testBits) == 0UL );
}

TEST_CASE( "set", "[signature][set]" )
{
  struct TestType1
  {
    int a = 1;
  };
  
  struct TestType2
  {
    int b = 1;
  };
  
  TEST::Signature signature;

  signature.set<TestType1, TestType2>();

  std::bitset<VECS_LIMIT> testBits;
  testBits.set(VECS_SETTINGS.component_id<TestType1>());
  testBits.set(VECS_SETTINGS.component_id<TestType2>());

  CHECK( (signature.bitset() & testBits) == testBits );
}

TEST_CASE( "unset", "[signature][unset]" )
{
  struct TestType1
  {
    int a = 1;
  };
  
  struct TestType2
  {
    int b = 1;
  };

  struct TestType3
  {
    int c = 1;
  };

  TEST::Signature signature;
  std::bitset<VECS_LIMIT> testBits1, testBits2, testBits3;

  testBits1.set(VECS_SETTINGS.component_id<TestType1>());
  testBits2.set(VECS_SETTINGS.component_id<TestType2>());
  testBits3.set(VECS_SETTINGS.component_id<TestType3>());

  signature.set<TestType1, TestType2, TestType3>();
  
  SECTION( "unset_all" )
  {
    auto testBits = testBits1 & testBits2 & testBits3;
    signature.unset<TestType1, TestType2, TestType3>();

    CHECK( (signature.bitset() & testBits) == 0UL );
  }

  SECTION( "unset_one" )
  {
    auto testBits = testBits1 & testBits2;
    signature.unset<TestType3>();

    CHECK( (signature.bitset() & testBits) == testBits );
  }

  SECTION( "unset_multiple" )
  {
    signature.unset<TestType2, TestType3>();

    CHECK( (signature.bitset() & testBits1) == testBits1 );
  }
}

TEST_CASE( "operator&", "[signatures][operator&]" )
{
  struct TestType1
  {
    int a = 1;
  };
  
  struct TestType2
  {
    int b = 1;
  };

  struct TestType3
  {
    int c = 1;
  };
  
  TEST::Signature signature1, signature2;

  signature1.set<TestType1, TestType2>();
  signature2.set<TestType1, TestType2, TestType3>();

  CHECK( !((signature1.bitset() & signature2.bitset()) == signature2.bitset()) );
  CHECK( (signature2.bitset() & signature1.bitset()) == signature1.bitset() );
}

TEST_CASE( "operator==", "[signatures][operator==]" )
{
  struct TestType
  {
    int a = 1;
  };

  TEST::Signature signature1, signature2;
  
  signature1.set<TestType>();
  signature2.set<TestType>();

  CHECK( signature1 == signature2 );
}