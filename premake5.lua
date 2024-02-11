workspace "PECS Library"
  configurations { "default" }
  
  project "PECS-Core"
    location "src/core"
    filename "Core"
    
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
   
    targetdir "lib"
    objdir "bin"
    targetname "pecs-core"

    files {
      "src/core/*.cpp",
      "src/core/include/*.hpp"
    }

    includedirs {
      ".",
      "/opt/homebrew/include",
      "/usr/local/include"
    }

  project "PECS-Objects"
    location "src/objects"
    filename "Objects"

    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    targetdir "lib"
    objdir "bin"
    targetname "pecs-objs"

    files {
      "src/objects/*.cpp",
      "src/objects/include/*.hpp"
    }

    includedirs {
      ".",
      "/opt/homebrew/include",
      "/usr/local/include"
    }

workspace "PECS Samples"
  configurations { "default" }
  location "samples"

  project "BasicShapes"
    location "samples/basic_shapes"

    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    targetdir "samples/bin"
    objdir "samples/obj"
    targetname "shapes"

    files {
      "samples/basic_shapes/shapes.cpp"
    }

    includedirs {
      "include",
      "/opt/homebrew/include",
      "/usr/local/include"
    }

    libdirs {
      "lib",
      "/opt/homebrew/lib",
      "/usr/local/lib"
    }

    links {
      "glfw",
      "vulkan",
      "glm",
      "pecs-core",
      "pecs-objs"
    }

    linkoptions { "-rpath /usr/local/lib" }