#!/bin/zsh

source .helpers.zsh

log "VECS headers version ${VERSION} started at ${TIME} on system ${SYS}"


while getopts ":s" opt
do
  case $opt in
    s)
      set_alias
      ;;
    \?)
      ;;
  esac
done


log "generating ${FILES[$FILE]}"

clear

input "// vecs::vecs_master_hpp version ${VERSION} generated on ${TIME} with system ${SYS}"

space

input "#ifndef vecs_master_hpp"
input "#define vecs_master_hpp"

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

input "#define VECS_LIMIT     std::numeric_limits<unsigned short>::max()"
input "#define VECS_SETTINGS  vecs::Settings::instance()"

space

input "namespace vecs"
input "{"

space

read_misc extras 7 49

space

for ELEMENT in "${SRCS[@]}"
do
  if [[ "${ELEMENT}" == "gui" ]]
  then
    read_file $ELEMENT "GUI"
  elif [[ "${ELEMENT}" == "entities" ]]
  then
    read_file $ELEMENT "EntityManager"
  elif [[ "${ELEMENT}" == "components" ]]
  then
    read_file $ELEMENT "IComponentArray"
    space
    read_file $ELEMENT "ComponentArray" 1
    space
    read_file $ELEMENT "ComponentManager"
  elif [[ "${ELEMENT}" == "systems" ]]
  then
    read_file $ELEMENT "System"
    space
    read_file $ELEMENT "SystemManager"
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

input "#include \"./templates.hpp\""

end_file "#endif // vecs_master_hpp"

FILE=2
log "generating ${FILES[$FILE]}"

clear

input "// vecs::vecs_templates_hpp version ${VERSION} generated on ${TIME} with system ${SYS}"

space

input "#ifndef vecs_templates_hpp"
input "#define vecs_templates_hpp"

space

input "namespace vecs"
input "{"

space

read_misc components_templates 4 110

space

read_misc entities_templates 4 47

space

read_misc settings_templates 4 13

space

read_misc signature_templates 4 34

space

read_misc systems_templates 4 74

space

input "} // namespace vecs"

space

end_file "#endif // vecs_templates_hpp"

log "done."