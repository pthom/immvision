add_executable(immdebug_demo immdebug_demo.cpp ../mandelbrot_demo_cpp/nlohmann_json.hpp)
target_link_libraries(immdebug_demo PRIVATE immdebug)
