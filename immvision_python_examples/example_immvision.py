#!/usr/bin/env python3
import imgui
import immvision
import imgui
import numpy as np
import cv2
import time
import os


THIS_DIR = os.path.dirname(__file__)


def static_vars(**kwargs):
    def decorate(func):
        for k in kwargs:
            setattr(func, k, kwargs[k])
        return func

    return decorate


# cf cf https://www.learnpythonwithrune.org/numpy-compute-mandelbrot-set-by-vectorization/
def mandelbrot(height, width, x=-0.5, y=0, zoom=1, max_iterations=100):
    # To make navigation easier we calculate these values
    x_width = 1.5
    y_height = 1.5 * height / width
    x_from = x - x_width / zoom
    x_to = x + x_width / zoom
    y_from = y - y_height / zoom
    y_to = y + y_height / zoom
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
        z[m] = z[m] ** 2 + c[m]
        diverged = np.greater(np.abs(z), 2, out=np.full(c.shape, False), where=m)  # Find diverging
        div_time[diverged] = i  # set the value of the diverged iteration number

        # m[np.abs(z) > 2] = False    # to remember which have diverged
        m[diverged] = False

    return div_time


@static_vars(video_capture=None, visible=False)
def show_camera():
    if show_camera.video_capture is None:
        show_camera.video_capture = cv2.VideoCapture(0)

    # Live video
    _, show_camera.visible = imgui.checkbox("Show camera", show_camera.visible)
    if show_camera.visible:
        video_ok, video_frame = show_camera.video_capture.read()
        if video_ok:
            immvision.image_display("video", video_frame, image_display_size=(0, 300), refresh_image=True)
            immvision.power_save_set_max_wait_next_frame(1 / 50)


start_time = time.time_ns()
image = cv2.imread(f"{THIS_DIR}/house.jpg")
image = np.random.rand(100, 100, 3)
image = mandelbrot(800, 1000)
image = image.astype(np.int32)


def _test_gui_function(params: immvision.ImguiAppParams):
    imgui.text(immvision.version_info())
    show_camera()
    # imgui.same_line()
    immvision.image_display("image", image, image_display_size=(0, 300))

    elapsed = (time.time_ns() - start_time) / 1e9
    imgui.text(f"elapsed time: {elapsed:.2f}s FPS: {imgui.get_io().framerate:.2f}")
    if imgui.button("Exit"):
        params.app_shall_exit = True


def draw_coords_markers(w, h):
    step = 100
    for x in range(0, w + step, step):
        for y in range(0, h + step, step):
            imgui.set_cursor_pos((x, y))
            imgui.text(f"{x=} {y=}")


def gui_test_autosize(params: immvision.ImguiAppParams):
    draw_coords_markers(100, 700)
    if imgui.button("Exit"):
        params.app_shall_exit = True


def main_autosize():
    params = immvision.ImguiAppParams()
    # params.use_power_save = False

    def my_gui():
        gui_test_autosize(params)

    immvision.run(my_gui, params)
    # immvision.run(my_gui, params)
    # immvision.run(my_gui, params)


def main():
    params = immvision.ImguiAppParams()
    # params.use_power_save = False

    def my_gui():
        _test_gui_function(params)

    immvision.run(my_gui, params)


if __name__ == "__main__":
    main()
    # main_autosize()
