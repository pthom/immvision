#!/usr/bin/env python3

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
    import _core_immvision_pybind as immvision;
else:
    import immvision;


image = cv2.imread("/Users/pascal/dvp/OpenSource/ImGuiWork/immvision/src/immvision_demos/inspector_demo/assets/reddit_place_2022.png")

def _test_gui_function(params: imgui_runner.ImguiAppParams):
    imgui.text("Hello there!")
    immvision.Image(image, True, (600, 600), True)
    if imgui.button("Exit"):
        params.app_shall_exit = True


if __name__ == "__main__":
    params = imgui_runner.ImguiAppParams()

    def my_gui():
        _test_gui_function(params)

    params.gui_function = my_gui
    imgui_runner.run(params)

