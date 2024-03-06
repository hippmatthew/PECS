workspace "PECS-Library"
  configurations { "default" }
  location "."
  
  project "PECS"
    location "src"
    filename "Core"
    
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
   
    targetdir "lib"
    objdir "bin"
    targetname "pecs"

    files {
      "src/core/*.cpp",
      "src/objects/*.cpp"
    }

    includedirs {
      ".",
      "/opt/homebrew/include",
      "/usr/local/include"
    }

  project "Test"
    location "test"

    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    targetdir "test"
    objdir "test/obj"
    targetname "test"

    files {
      "test/src/*.cpp"
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
      "pecs"
    }

    linkoptions { "-rpath /usr/local/lib" }