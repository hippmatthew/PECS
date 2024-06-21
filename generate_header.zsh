#!/bin/zsh

source .helpers.zsh

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

read_file extras $START $END

newline

for ELEMENT in "${FILES[@]}"
do
  read_file $ELEMENT
  newline
done

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

end_file "#endif // vecs_hpp"