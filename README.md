# P E C S

**P**hysics **E**ngine _for_ **C**omputation _and_ **S**imulations, _By Matthew Hipp_

PECGS is a physics engine designed for gpu accelerated physics computation and simulation using VulkanSDK.
THe idea is to have a separate compuational and graphical components which accesss the GPU's compute or graphics queue family respectively. The engine itself will contain the window, vulkan instance, physical device, and logical deice as well as access to the graphical and computational components if one wishes to use them.

Current Supported Vulkan Version: **_1.3.250.0_**

## Features Currently Being Worked On

_Unfinished material is located in the W.I.P branch_

0. Initial Setup

   - [x] local directories for archive output, binaries, object files, src files, and shaders
   - [x] default environment file for external library and local directories
   - [x] default makefile setup
   - [x] solution to setup the engine for Windows or MacOS

1. Engine Object

   - [ ] Vulkan Instance
   - [ ] Validation Layers
   - [ ] Command Pool / Command Buffers
   - [ ] Sync Objects
   - [ ] Frame Drawing
   - [ ] Main Engine Loop
   - [ ] CONTAINS: device, window, and pipeline object

2. Window Object

   - [ ] GLFW Window
   - [ ] Window Surface

3. Device Object

   - [ ] Physical Device
   - [ ] Logical Device
   - [ ] Device Queue Families

4. Graphics Pipeline Object

   - [ ] Swapchain
   - [ ] Swapchain Image Views
   - [ ] Swapchain Images
   - [ ] Render Pass
   - [ ] Framebuffers
   - [ ] Pipeline

5. Debug Utils Object

   - [ ] toggle based on -DDEBUG macro
   - [ ] Mediates all shell output

## Future Features to be Developed

_Features I would like to add after finishing the main functionality of the engine_

1. Vertex Buffers
2. Uniform Buffers
3. Texture Mapping
4. Depth Buffers
5. Mipmaps
6. Multisampling
7. Rigidbody Primitives (Cube, Cylinder, Sphere, Point)
8. Fields
9. Object importing
10. Mass Density Maps (like a texture map but for mass density. Useful for creating more complex rigidbodies)
11. Fluids
12. GUI (if needed)

## Windows Setup

**MAKE SURE TO INSTALL MINGW64 AND VULKANSDK**

1. Open the MSYS2 MINGW64 shell
2. Use `pacman -S mingw-w64-x86_64-glfw3` to install GLFW
3. Use `pacman -S mingw-w64-x86_64-glm` to install GLM
4. Clone the repository and store it in an accessible location on the system
5. Remove .default from the .env file and open it
6. Update `VULKAN_SDK_DIR` with the directory of the desired VulkanSDK version
7. Update `MINGW_DIR` with the directory of the mingw64 folder
8. Comment out all MacOS related variables
9. Notice that the Windows paths have forward slashes. This is on purpose, due to the want of having a uniform shell output. Feel free to change them to backward slahses.
10. Remove .default from the makefile and open it
11. Set `VULKAN_INCLUDE_DIR` equal to `WIN_VULKAN_INCLUDE_DIR`
12. Set `VULKAN_LIB_DIR` equal to `WIN_VULKAN_LIB_DIR`
13. Set `VULKAN_LIB` equal to `WIN_VULKAN_LIB`
14. Set `EXT_INCLUDE_DIR` equal to `WIN_INCLUDE_DIR`
15. Uncomment the `LDFLAGS` statement on line 18 under "Windows GLFW Library Linker"
16. Uncomment the Windows clean functions at the bottom of the makefile
17. Add a "obj" and "bin" directory to the project folder. These will house the object files and binaries respectively
18. (optional) Remove all MacOS related variables and comments

## MacOS Setup

**MAKE SURE TO INSTALL HOMEBREW AND VULKANSDK**

1. Open terminal
2. Use `brew install glfw` to install GLFW
3. Use `brew install glm` to install GLM
4. Use `brew install pkg-config` to install pkg-config
5. Clone the repository and store it in an accessible location on the system
6. Remove .default from the .env file and open it
7. Update `VULKAN_SDK_DIR` with the directory of the desired VulkanSDK version
8. Update `BREW_DIR` with the directory of the homebrew installation. The suggested directory is for Apple Silicon users only
9. Comment out all Windows related variables
10. Remove .default from the makefile and open it
11. Set `VULKAN_INCLUDE_DIR` equal to `MACOS_VULKAN_INCLUDE_DIR`
12. Set `VULKAN_LIB_DIR` equal to `MACOS_VULKAN_LIB_DIR`
13. Set `VULKAN_LIB` equal to `MACOS_VULKAN_LIB`
14. Set `EXT_INCLUDE_DIR` equal to `MACOS_INCLUDE_DIR`
15. Uncomment the `CFLAGS_DEFAULT` and `LDFLAGS` statements on lines 21 and 22 under "MacOS GLFW Library Linker"
16. Uncomment the MacOS clean functions at the bottom of the makefile
17. Create a directory named "obj" in the project folder. This will house the object files
18. (optional) Remove all Windows related variables and comments

## Makefile Commands

|         Code          | Functionality                                                       |
| :-------------------: | :------------------------------------------------------------------ |
| `make`, `make debug`  | compiles the object files in debug mode                             |
|    `make release`     | compiles the object files without debug mode                        |
| `make static_library` | archives the object files                                           |
|    `make shaders`     | manually compiles the vertex and fragment shaders into SPIR-V files |
|     `make clean`      | removes all binaries, object files, and SPIR-V files                |
| `make clean_shaders`  | removes all SPIR-V files                                            |
