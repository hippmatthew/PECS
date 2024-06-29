#!/bin/zsh

source .helpers.zsh

clear

input "// vecs_hpp version ${VERSION} generated on ${TIME}"

space

input "#ifndef vecs_hpp"
input "#define vecs_hpp"

space

input "#ifndef vecs_no_includes"

space

input "#define VULKAN_HPP_NO_CONSTRUCTORS"
input "#include <vulkan/vulkan_raii.hpp>"

space

input "#define GLFW_INCLUDE_NONE"
input "#include <GLFW/glfw3.h>"

space

input "#define GLM_FORCE_RADIANS"
input "#include <glm/glm.hpp>"
input "#include <glm/gtc/matrix_transform.hpp>"

space

input "#endif // vecs_no_includes"

space

for ELEMENT in "${DEPS[@]}"
do
  input "#include <${ELEMENT}>"
done

space

input "namespace vecs"
input "{"

space

read_extras

space

for ELEMENT in "${FILES[@]}"
do
  if [[ "${ELEMENT}" == "gui" ]]
  then
    read_file $ELEMENT "GUI"
  elif [[ "${ELEMENT}" == "entities" ]]
  then
    read_file $ELEMENT "EntityManager"
  else
    read_file $ELEMENT
  fi

  space
done

input "FamilyType to_family(unsigned int);"
input "unsigned int to_bits(FamilyType);"

space

input "} // namespace vecs"

space

input "namespace std"
input "{"

space

input "std::string to_string(vecs::FamilyType);"

space

input "} // namspace std"

space

end_file "#endif // vecs_hpp"