#!/usr/bin/env python3
# import _pybind_libs_path
import imgui
import immvision.imgui_runner as imgui_runner

import imgui
import sdl2
import numpy as np
import time

image = np.zeros([800, 600, 3], np.uint8)

start_time = time.time_ns()


def _test_gui_function(params: imgui_runner.ImguiAppParams):
    if imgui.button("Exit"):
        params.app_shall_exit = True

    elapsed = (time.time_ns() - start_time) / 1E9
    imgui.text(f"elapsed time: {elapsed:.2f}s FPS: {imgui.get_io().framerate:.2f}")



def main():
    params = imgui_runner.ImguiAppParams()
    # params.use_power_save = False

    def my_gui():
        _test_gui_function(params)

    params.gui_function = my_gui
    imgui_runner.run(params)


if __name__ == "__main__":
    main()
