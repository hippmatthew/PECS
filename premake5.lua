workspace "PECS Library"
  configurations { "default" }

  project "PECS Core"
    location "src"
    filename "Core"
    
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
   
    targetdir "lib"
    objdir "bin"
    targetname "pecs-core"

    files {
      "src/*.cpp",
      "src/include/*.hpp"
    }

    includedirs {
      ".",
      "/opt/homebrew/include",
      "/usr/local/include"
    }

workspace "PECS Samples"
  configurations { "default" }
  location "samples"

  project "Triangles"
    location "samples/triangles"

    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    targetdir "samples/bin"
    objdir "samples/obj"
    targetname "triangles"

    files {
      "samples/triangles/triangles.cpp"
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
      "pecs-core"
    }

    linkoptions { "-rpath /usr/local/lib" }