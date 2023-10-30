# P E C S

**P**hysics **E**ngine _for_ **C**omputation _and_ **S**imulation, _By Matthew Hipp_

PECS is a physics engine designed for gpu accelerated physics computation and simulation using VulkanSDK.
The idea is to have a separate compuational and graphical components which accesss the GPU's compute or graphics queue family respectively. The engine itself will contain the window, vulkan instance, physical device, and logical device as well as access to the graphical and computational components if one wishes to use them.

Current Supported Vulkan Version: **_1.3.268.1_**
