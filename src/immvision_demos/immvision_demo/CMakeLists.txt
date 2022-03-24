add_executable(immvision_demo
    immvision_demo.cpp
    mandelbrot.h
    mandelbrot.cpp
    )
target_link_libraries(immvision_demo PRIVATE immvision_simple_runner immvision)
