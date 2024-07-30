cmake_minimum_required(VERSION 3.20.0)
project(vecs VERSION 1.4)
option(CMAKE_EXPORT_COMPILE_COMMANDS "Export compile_commands.json for IDEs" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER clang++)

include_directories(
  ${CMAKE_SOURCE_DIR}/.
  /opt/homebrew/include
  /usr/local/include
)

set(SOURCES
  ${CMAKE_SOURCE_DIR}/src/core/include/components_templates.hpp
  ${CMAKE_SOURCE_DIR}/src/core/include/entities_templates.hpp
  ${CMAKE_SOURCE_DIR}/src/core/include/settings_templates.hpp
  ${CMAKE_SOURCE_DIR}/src/core/include/signature_templates.hpp
  ${CMAKE_SOURCE_DIR}/src/core/include/systems_templates.hpp
  ${CMAKE_SOURCE_DIR}/src/core/device.cpp
  ${CMAKE_SOURCE_DIR}/src/core/engine.cpp
  ${CMAKE_SOURCE_DIR}/src/core/entities.cpp
  ${CMAKE_SOURCE_DIR}/src/core/gui.cpp
  ${CMAKE_SOURCE_DIR}/src/core/settings.cpp
  ${CMAKE_SOURCE_DIR}/src/core/signature.cpp
  ${CMAKE_SOURCE_DIR}/src/core/systems.cpp
)

add_library(vecs STATIC ${SOURCES})

set_target_properties(vecs PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
)