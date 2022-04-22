#!/usr/bin/env python3
import imgui
import immvision
import immvision.cpp_immvision
import immvision.imgui_runner as imgui_runner
import imgui
import cv2
import time
import os


def main():
    print("********* pip_test_app: graphical test app that exits after 5 seconds *********")
    params = imgui_runner.ImguiAppParams()
    THIS_DIR = os.path.dirname(__file__)

    image = cv2.imread(f"{THIS_DIR}/house.jpg")
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
        imgui.text(immvision.cpp_immvision.VersionInfo())
        imgui.text(f"FPS: {imgui.get_io().framerate:.2f}")
        immvision.ImageNavigator(image)
        if chrono < 0:
            print("********* pip_test_app: exit *********")
            params.app_shall_exit = True

    params.gui_function = my_gui
    imgui_runner.run(params)


if __name__ == "__main__":
    main()
