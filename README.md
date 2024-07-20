# **V**ulkan **E**ngine *for* **C**omputation *and* **S**imulation

### Dependencies

- **Vulkan :** 1.3.283.0
- **GLFW :** 3.3.8
- **GLM :** 0.9.9.8
- **Premake5 :** 5.0.0
- **Catch2 :** 3.5.3
- **zsh** and **clang++** are recommended

### Installation

1. Make sure that the dependencies are installed on your machine. For Linux, install using your package manager. For MacOS, install with homebrew
2. Run `zsh generate_header.zsh` script to generate the master header files located in `/include`
3. Run premake with `premake5 <build-style>`
  - To generate makefiles, replace `<build-style>` with `gmake2`
  - To generate an XCode project, replace `<build-style>` with `xcode4`
  - To generate a Visual Studio project, replace `<build-style>` with `vsxxxx` where *xxxx* is your visual studio year (ex: Visual Studio 2022 projects would use `vs2022`)
4. Build with your respective build system

**Note:** VECS has not been tested on XCode or Visual Studio. It has only been built with Makefiles

### Using VECS

*This section is in the process of being written...*