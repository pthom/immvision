// stb_image implementation for standalone immvision builds.
// When built with hello_imgui, the implementation is already compiled there,
// so this file compiles to nothing (IMMVISION_PROVIDE_STB_IMAGE is not defined).
#ifdef IMMVISION_PROVIDE_STB_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif
