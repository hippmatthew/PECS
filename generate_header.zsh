#!/bin/zsh

source .helpers.zsh

FILE=include/vecs.hpp
VERSION="0.0.6.5"
TIME=$(date "+%m-%d-%Y %H:%M:%S")

EXTRAS_START=7
EXTRAS_END=42
SETTINGS_START=17
SETTINGS_END=95
MAT_START=13
MAT_END=66
GUI_START=20
GUI_END=63
DEVICE_START=23
DEVICE_END=100
SYNC_START=9
SYNC_END=44
ENGINE_START=9
ENGINE_END=33

clear

input "// vecs_hpp version ${VERSION} generated on ${TIME}"

newline

input "#ifndef vecs_hpp"
input "#define vecs_hpp"

newline

input "#ifndef vecs_include_dependencies"
input "#define vecs_include_dependencies"

newline

input "#define VULKAN_HPP_NO_CONSTRUCTORS"
input "#include <vulkan/vulkan_raii.hpp>"

newline

input "#define GLFW_INCLUDE_NONE"
input "#include <GLFW/glfw3.h>"

newline

input "#define GLM_FORCE_RADIANS"
input "#include <glm/glm.hpp>"
input "#include <glm/gtc/matrix_transform.hpp>"

newline

input "#endif // vecs_include_dependencies"

newline

input "#include <map>"
input "#include <memory>"
input "#include <string>"
input "#include <vector>"

newline

input "namespace vecs"
input "{"

newline

read_file extras $EXTRAS_START $EXTRAS_END

newline

read_file settings $SETTINGS_START $SETTINGS_END

newline

read_file material $MAT_START $MAT_END

newline

read_file gui $GUI_START $GUI_END

newline

read_file device $DEVICE_START $DEVICE_END

newline

read_file synchronization $SYNC_START $SYNC_END

newline

read_file engine $ENGINE_START $ENGINE_END

newline

input "FamilyType to_family(unsigned int);"
input "unsigned int to_bits(FamilyType);"

input "} // namespace vecs"

newline

input "namespace std"
input "{"

newline

input "std::string to_string(vecs::FamilyType);"

newline

input "} // namspace std"

newline

input "#endif // vecs_hpp"