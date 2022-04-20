#!/usr/bin/env python3
import imgui
import immvision
import immvision.imgui_runner as imgui_runner
import imgui
import cv2
import sdl2
import numpy as np
import time
import os

THIS_DIR = os.path.dirname(__file__)
image = cv2.imread(f"{THIS_DIR}/smiley.png")

# video_capture = cv2.VideoCapture(0)
# show_video = True
start_time = time.time_ns()


def _test_gui_function(params: imgui_runner.ImguiAppParams):
    # global show_video
    # # Live video
    # _, show_video = imgui.checkbox("Show video", show_video)
    # if show_video:
    #     video_ok, video_frame = video_capture.read()
    #     if video_ok:
    #         immvision.Image(video_frame, True, (500, 0), True)
    #         imgui.same_line()
    #         imgui_runner.power_save.set_max_wait_before_next_frame(1/50)

    immvision.Image(image, False, (10, 0), True); imgui.same_line()
    immvision.ImageNavigator(image)
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
