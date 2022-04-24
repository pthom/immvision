#!/usr/bin/env python3
import imgui
import immvision
import immvision.cpp_immvision
import immvision.imgui_runner as imgui_runner
import imgui
import numpy as np
import cv2
import time
import os

THIS_DIR = os.path.dirname(__file__)
image = cv2.imread(f"{THIS_DIR}/house.jpg")
image = np.random.rand(100,100,3)

#video_capture = cv2.VideoCapture(0)
#show_camera = False
start_time = time.time_ns()

# cf cf https://www.learnpythonwithrune.org/numpy-compute-mandelbrot-set-by-vectorization/
def mandelbrot(height, width, x=-0.5, y=0, zoom=1, max_iterations=100):
    # To make navigation easier we calculate these values
    x_width = 1.5
    y_height = 1.5*height/width
    x_from = x - x_width/zoom
    x_to = x + x_width/zoom
    y_from = y - y_height/zoom
    y_to = y + y_height/zoom
    # Here the actual algorithm starts
    x = np.linspace(x_from, x_to, width).reshape((1, width))
    y = np.linspace(y_from, y_to, height).reshape((height, 1))
    c = x + 1j * y
    # Initialize z to all zero
    z = np.zeros(c.shape, dtype=np.complex128)
    # To keep track in which iteration the point diverged
    div_time = np.zeros(z.shape, dtype=int)
    # To keep track on which points did not converge so far
    m = np.full(c.shape, True, dtype=bool)
    for i in range(max_iterations):
        z[m] = z[m]**2 + c[m]
        diverged = np.greater(np.abs(z), 2, out=np.full(c.shape, False), where=m) # Find diverging
        div_time[diverged] = i      # set the value of the diverged iteration number

        #m[np.abs(z) > 2] = False    # to remember which have diverged
        m[diverged] = False

    return div_time


m = np.zeros((2, 4, 6), dtype = int)
image = mandelbrot(800, 1000)
image = image.astype(np.int32)

enumerate


def _test_gui_function(params: imgui_runner.ImguiAppParams):
    # global show_camera
    # # Live video
    # _, show_camera = imgui.checkbox("Show camera", show_camera)
    # if show_camera:
    #     video_ok, video_frame = video_capture.read()
    #     if video_ok:
    #         immvision.Image(video_frame, True, (500, 0), True)
    #         imgui.same_line()
    #         imgui_runner.power_save.set_max_wait_before_next_frame(1/50)

    # immvision.Image(image, False, (300, 0), True); imgui.same_line()
    imgui.text(immvision.cpp_immvision.VersionInfo())
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
