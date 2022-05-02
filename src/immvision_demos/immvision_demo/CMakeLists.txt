set(CMAKE_CXX_STANDARD 20)
if (WIN32)
  # opencv2/videoio.hpp(311,80): warning C5054: operator '+': deprecated between enumerations of different types
  #message : to simplify migration, consider the temporary use of /Wv:18 flag with the version of the compiler
  # with which you used to build without warnings
  # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} /Wv:18)
endif()
include(${HELLOIMGUI_BASEPATH}/hello_imgui_cmake/hello_imgui_add_app.cmake)
hello_imgui_add_app(immvision_demo immvision_demo.cpp)
target_link_libraries(immvision_demo PRIVATE immvision)
