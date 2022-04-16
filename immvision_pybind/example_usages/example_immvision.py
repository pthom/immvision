#!/usr/bin/env python3

#  export PYTHONPATH="/Users/pascal/dvp/OpenSource/ImGuiWork/immvision/cmake-build-debug/immvision_pybind:src/immvision/

import imgui
import imgui_runner
import imgui
import cv2
import sdl2
import numpy as np

DEV_MODE = True
if DEV_MODE:
    import os
    import sys
    sys.path.append(os.getcwd())
    import _core_immvision_pybind as immvision
else:
    import immvision;

folder = "/Users/pascal/dvp/OpenSource/ImGuiWork/immvision/src/immvision_demos/inspector_demo/assets/"
image = cv2.imread(f"{folder}/reddit_place_2022.png")

def _test_gui_function(params: imgui_runner.ImguiAppParams):
    imgui.text("Hello there!")
    # immvision.Image(image, True, (150, 150), True)
    immvision.ImageNavigator(image)
    if imgui.button("Exit"):
        params.app_shall_exit = True


def main():
    params = imgui_runner.ImguiAppParams()

    def my_gui():
        _test_gui_function(params)

    params.gui_function = my_gui
    imgui_runner.run(params)


if __name__ == "__main__":
    main()
