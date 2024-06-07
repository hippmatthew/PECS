workspace "PECS-Library"
  configurations { "default" }
  platforms { "MacOS-ARM", "Linux-x86_64" }
  location "."
  
  filter { "platforms:MacOS-ARM" }
    system "macosx"
    architecture "ARM64"

  filter { "platforms:Linux-x86_64" }
    system "linux"
    architecture "x86_64"
  
  project "PECS"
    location "src"
    filename "PECS"
    
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

    includedirs { "." } 
    
    filter { "platforms:MacOS-ARM" }
      includedirs {
        "/opt/homebrew/include",
        "/usr/local/include"
      }

  project "Test-Program"
    location "test"
    filename "Test"

    kind "ConsoleApp"
    language "C++"
    cppdialect "c++20"

    targetdir "test/bin"
    objdir "test/obj"
    targetname "output"

    files {
      "test/src/*.cpp"
    }

    includedirs {
      ".",
      "./include"
    }

    libdirs { "lib" }

    links {
      "vulkan",
      "glfw",
      "pecs"
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