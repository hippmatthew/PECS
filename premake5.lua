workspace "VECS-Library"
  configurations { "default" }
  platforms { "MacOS-ARM", "Linux-x86_64" }
  location "."
  
  filter { "platforms:MacOS-ARM" }
    system "macosx"
    architecture "ARM64"

  filter { "platforms:Linux-x86_64" }
    system "linux"
    architecture "x86_64"
  
  project "VECS"
    location "src"
    filename "VECS"
    
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
   
    targetdir "lib"
    objdir "bin"
    targetname "vecs"

    files {
      "src/core/include/*.hpp",
      "src/core/*.cpp"
    }

    includedirs { "." } 
    
    filter { "platforms:MacOS-ARM" }
      includedirs {
        "/opt/homebrew/include",
        "/usr/local/include"
      }

  project "Tests"
    location "tests"
    filename "Tests"

    kind "ConsoleApp"
    language "C++"
    cppdialect "c++20"

    targetdir "."
    objdir "bin/tests"
    targetname "test"

    files {
      "tests/*.hpp",
      "tests/*.cpp"
    }

    includedirs {
      ".",
      "./include"
    }

    libdirs { "lib" }

    links {
      "vulkan",
      "glfw",
      "catch2",
      "vecs"
    }

    filter { "platforms:MacOS-ARM" }
      includedirs {
        "/opt/homebrew/include",
        "/usr/local/include"
      }

      libdirs {
        "/opt/homebrew/lib",
        "/usr/local/lib"
      }

      linkoptions { "-rpath /usr/local/lib" }