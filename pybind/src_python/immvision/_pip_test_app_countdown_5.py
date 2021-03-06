#!/usr/bin/env python3

"""
_pip_test_app_countdown_5: graphical test app that exits after 5 seconds

used only for internal tests
"""

import imgui
import immvision
import immvision._cpp_immvision
import imgui
import cv2
import time
import os
import numpy as np


def main():
    print("********* _pip_test_app_countdown_5: graphical test app that exits after 5 seconds *********")
    params = immvision.ImguiAppParams()
    THIS_DIR = os.path.dirname(__file__)

    image = np.zeros((800, 600, 3), np.uint8)
    start_time = -1

    def chrono_countdown(total_duration):
        nonlocal start_time
        if start_time < 0:
            start_time = time.time()
        elapsed = time.time() - start_time
        return total_duration - elapsed

    def my_gui():
        chrono = chrono_countdown(3)
        imgui.text(f"Chrono: {chrono:.2f}s")
        imgui.new_line()
        imgui.text(immvision._cpp_immvision.version_info())
        imgui.text(f"FPS: {imgui.get_io().framerate:.2f}")
        immvision.image_display("image", image)
        if chrono < 0:
            print("********* pip_test_app: exit *********")
            params.app_shall_exit = True

    params.gui_function = my_gui
    immvision.run(my_gui, params)


main()
