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

   - [x] GLFW Window
   - [x] Window Surface

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

1. Install Vulkan (https://vulkan.lunarg.com)
2. Install MinGW-w64 (https://www.mingw-w64.org)
3. Open the MSYS2 MINGW64 shell
4. Use `pacman -S mingw-w64-x86_64-glfw3` to install GLFW
5. Use `pacman -S mingw-w64-x86_64-glm` to install GLM
6. Clone the repository and store it in an accessible location on the system
7. Remove .default from the .env file and open it
8. Update `VULKAN_SDK_DIR` with the directory of the desired VulkanSDK version
9. Update `MINGW_DIR` with the directory of the mingw64 folder
10. Notice that the Windows paths have forward slashes. This is on purpose, due to the want of having a uniform shell output. Feel free to change them to backward slashes. It does not matter.
11. Remove .default from the makefile and open it
12. Uncomment the windows specific variables
13. Delete all MacOS specific variables
14. Uncomment the Windows clean functions at the bottom of the makefile and remove the MacOs clean functions

## MacOS Setup

**MAKE SURE TO INSTALL HOMEBREW AND VULKANSDK**

1. Install Vulkan (https://vulkan.lunarg.com)
2. Open terminal
3. Use `cd ~/VulkanSDK/v.w.xx.0` (v.w.xx.0 is the vulkan version number. Change according to vulkan version and respective installation directory)
4. Use `python3 install_vulkan.py`
5. Install Homebrew (https://brew.sh)
6. Open terminal
7. Use `brew install glfw` to install GLFW
8. Use `brew install glm` to install GLM
9. Use `brew install pkg-config` to install pkg-config
10. Clone the repository and store it in an accessible location on the system
11. Remove .default from the .env file and open it
12. Delete all Windows specific variables
13. Remove .default from the makefile and open it
14. Uncomment the MacOS specific variables
15. Delete all Windows specific variables
16. Uncomment the MacOS clean functions at the bottom of the makefile and delete the Windows clean functions

## Makefile Commands

|         Code         | Functionality                                                                                                                                        |
| :------------------: | :--------------------------------------------------------------------------------------------------------------------------------------------------- |
| `make`, `make debug` | compiles the library in debug mode                                                                                                                   |
|    `make release`    | compiles a release version of the library                                                                                                            |
|    `make shaders`    | compiles the vertex and fragment shaders into SPIR-V files.Make sure to do this whenever shader files are updated and before you compile the library |
|     `make clean`     | removes all object files and SPIR-V files                                                                                                            |
|  `make clean_spvs`   | removes all SPIR-V files                                                                                                                             |
