#!/usr/bin/env python3

import imgui
import imgui_runner
import immvision

def _test_gui_function(params: imgui_runner.ImguiAppParams):
    imgui.text("Hello there!")
    if imgui.button("Exit"):
        params.app_shall_exit = True


if __name__ == "__main__":
    params = imgui_runner.ImguiAppParams()

    def my_gui():
        _test_gui_function(params)

    params.gui_function = my_gui
    imgui_runner.run(params)

