// stb_image_write implementation for standalone immvision builds.
// When built with hello_imgui, the implementation is already compiled there,
// so this file compiles to nothing (IMMVISION_PROVIDE_STB_IMAGE_WRITE is not defined).
#ifdef IMMVISION_PROVIDE_STB_IMAGE_WRITE
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif
