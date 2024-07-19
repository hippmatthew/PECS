#include "src/core/include/settings.hpp"

#include <catch2/catch_test_macros.hpp>

#include <string>
TEST_CASE( "update_name", "[settings][name]" )
{
  std::string testName = "test_name";
  VECS_SETTINGS.update_name(testName);

  REQUIRE( VECS_SETTINGS.name() == testName );
}

TEST_CASE( "update_version", "[settings][version]" )
{
  unsigned int testVersion = VK_MAKE_API_VERSION(1, 0, 1, 0);
  VECS_SETTINGS.update_version(testVersion);

  REQUIRE( VECS_SETTINGS.version() == testVersion );
}

TEST_CASE( "toggle_validation", "[settings][validation]" )
{
  bool previous = VECS_SETTINGS.validation_enabled();
  VECS_SETTINGS.toggle_validation();

  REQUIRE( VECS_SETTINGS.validation_enabled() != previous );
}

TEST_CASE( "update_title", "[settings][title]" )
{
  std::string testTitle = "test_title";
  VECS_SETTINGS.update_title(testTitle);

  REQUIRE( VECS_SETTINGS.title() == testTitle );
}

TEST_CASE( "update_width", "[settings][width]" )
{
  unsigned int testWidth = 100;
  VECS_SETTINGS.update_width(testWidth);

  REQUIRE( VECS_SETTINGS.width() == testWidth );
}

TEST_CASE( "update_height", "[settings][height]" )
{
  unsigned int testHeight = 100;
  VECS_SETTINGS.update_height(testHeight);

  REQUIRE( VECS_SETTINGS.height() == testHeight );
}

TEST_CASE( "toggle_portability", "[settings][portability]" )
{
  bool previous = VECS_SETTINGS.portability_enabled();
  VECS_SETTINGS.toggle_portability();

  REQUIRE( VECS_SETTINGS.portability_enabled() != previous );
}

TEST_CASE( "extensions", "[settings][extensions]" )
{
  SECTION( "retreive_extensions" )
  {
    std::vector<const char *> testExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
    };

    REQUIRE( VECS_SETTINGS.device_extensions() == testExtensions );
  }
  
  SECTION( "add_extension" )
  {
    std::string testExtension = "test_extension";
    unsigned long prevSize = VECS_SETTINGS.device_extensions().size();
    
    VECS_SETTINGS.add_device_extension(testExtension.c_str());

    REQUIRE( VECS_SETTINGS.device_extensions().size() == prevSize + 1 );
  }

  SECTION( "remove_extension" )
  {
    std::string testExtension = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    unsigned long prevSize = VECS_SETTINGS.device_extensions().size();

    VECS_SETTINGS.remove_device_extension(testExtension.c_str());

    REQUIRE( VECS_SETTINGS.device_extensions().size() == prevSize - 1 );
  }
}

TEST_CASE( "update_format", "[settings][format]" )
{
  vk::Format testFormat = vk::Format::eA1B5G5R5UnormPack16KHR;
  VECS_SETTINGS.update_format(testFormat);

  REQUIRE( VECS_SETTINGS.format() == testFormat);
}

TEST_CASE( "update_color_space", "[settings][colorspace]" )
{
  vk::ColorSpaceKHR testSpace = vk::ColorSpaceKHR::eAdobergbLinearEXT;
  VECS_SETTINGS.update_color_space(testSpace);

  REQUIRE( VECS_SETTINGS.color_space() == testSpace );
}

TEST_CASE( "update_present_mode", "[settings][presentmode]" )
{
  vk::PresentModeKHR testMode = vk::PresentModeKHR::eFifo;
  VECS_SETTINGS.update_present_mode(testMode);

  REQUIRE( VECS_SETTINGS.present_mode() == testMode );
}

TEST_CASE( "update_extent", "[settings][extent]" )
{
  vk::Extent2D testExtent{
    .width  = 100,
    .height = 100
  };

  VECS_SETTINGS.update_extent(100, 100);

  REQUIRE( VECS_SETTINGS.extent() == testExtent );
}

TEST_CASE( "update_flight_frames", "[settings][flightframes]" )
{
  unsigned int testFrames = 3;
  VECS_SETTINGS.update_max_flight_frames(testFrames);

  REQUIRE( VECS_SETTINGS.max_flight_frames() == testFrames );
}

TEST_CASE( "update_background", "[settings][background]" )
{
  vk::ClearColorValue testColor{std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
  VECS_SETTINGS.update_background_color(vk::ClearValue{testColor});

  REQUIRE( VECS_SETTINGS.background_color().color.float32 == testColor.float32 );
}

TEST_CASE( "update_entities", "[settings][entities]" )
{
  unsigned long testEntities = 10;
  VECS_SETTINGS.update_max_entities(testEntities);

  REQUIRE( VECS_SETTINGS.max_entities() == testEntities );
}

TEST_CASE( "update_components", "[settings][components]" )
{
  unsigned long testComponents = 10;
  VECS_SETTINGS.update_max_components(testComponents);

  REQUIRE( VECS_SETTINGS.max_components() == testComponents );
}

TEST_CASE( "defaults", "[settings][defaults]" )
{
  std::vector<const char *> testExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
  };

  vk::Extent2D testExtent{
    .width  = 1280,
    .height = 720
  };

  vk::ClearColorValue testColor{std::array<float, 4>{0.0025f, 0.01f, 0.005f, 1.0f}};
  
  VECS_SETTINGS.set_default();
  
  CHECK( VECS_SETTINGS.name() == "VECS Application" );
  CHECK( VECS_SETTINGS.version() == VK_MAKE_API_VERSION(0, 1, 0, 0) );
  CHECK( VECS_SETTINGS.validation_enabled() == true );
  CHECK( VECS_SETTINGS.title() == "VECS Application" );
  CHECK( VECS_SETTINGS.width() == 1280 );
  CHECK( VECS_SETTINGS.height() == 720 );
  CHECK( VECS_SETTINGS.portability_enabled() == false );
  CHECK( VECS_SETTINGS.device_extensions() == testExtensions );
  CHECK( VECS_SETTINGS.format() == vk::Format::eB8G8R8A8Srgb );
  CHECK( VECS_SETTINGS.color_space() == vk::ColorSpaceKHR::eSrgbNonlinear );
  CHECK( VECS_SETTINGS.present_mode() == vk::PresentModeKHR::eMailbox );
  CHECK( VECS_SETTINGS.extent() == testExtent );
  CHECK( VECS_SETTINGS.max_flight_frames() == 2 );
  CHECK( VECS_SETTINGS.background_color().color.float32 == testColor.float32 );
  CHECK( VECS_SETTINGS.max_entities() == 20000 );
  CHECK( VECS_SETTINGS.max_components() == 100 );
}