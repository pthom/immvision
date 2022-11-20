add_executable(demo_immdebug demo_immdebug.main.cpp ../demo_imm_mandelbrot/nlohmann_json.hpp)
target_link_libraries(demo_immdebug PRIVATE immdebug)
