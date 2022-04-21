// This file should reproduce closely the options of pyimgui:
// see https://raw.githubusercontent.com/pyimgui/pyimgui/master/config-cpp/py_imconfig.h
// It is "force included" during the compilation of immvision python module

// tune vertex index size to enable longer draw lists (see #138 issue)
#define ImDrawIdx unsigned int

//---- Use 32-bit for ImWchar (default is 16-bit) to support unicode planes 1-16. (e.g. point beyond 0xFFFF like emoticons, dingbats, symbols, shapes, ancient languages, etc...)
#define IMGUI_USE_WCHAR32

// Redefine IM_ASSERT to raise Python specific exceptions ===> impossible for immvision
// note: enabling asserts as Python exceptions guards us from
//       possible segmentation faults when using functions that
//       does not return error values. Especially when pushing/poping
//       style stack variables.
// #define IM_ASSERT(EX) (void)((EX) || (__py_assert ("ImGui assertion error (" #EX ") at " AT),0))
//void __py_assert(const char* msg);
