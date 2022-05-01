from dataclasses import dataclass
from typing import Tuple, Optional
import os

Float_Between_0_1 = float

APP_WINDOW_POS_INI_FILE = "imgui_app_window.ini"


@dataclass
class ImguiAppParams:
    # Title and Size
    app_window_title: str = "ImGui Application"
    # app_window_size: Application window size: if you do not change it,
    # the app window size will try to match the combined widgets size.
    app_window_size: Tuple[int, int] = (0, 0)
    # Provide a default full *imgui* window inside the app
    provide_default_window: bool = True
    # Make the App window almost full screen?
    windowed_full_screen: bool = False
    # Change this bool to True when you want to exit
    app_shall_exit: bool = False
    # Use power save: if this is true, you can increase manually the idle FPS by calling
    # imgui_runner.power_save.set_max_wait_before_next_frame( 1 / desired_fps)
    use_power_save: bool = True
    # Anti-aliasing number of samples
    # - if the call to SDL_GL_CreateContext fails, try reducing the value to zero
    # - if you want better antialiasing, you can also increase it
    gl_multisamples = 4
    # Background color (i.e glClearColor)
    background_color: Tuple[Float_Between_0_1, Float_Between_0_1, Float_Between_0_1, Float_Between_0_1] = (1., 1., 1., 1)


def get_app_window_size_initial(imgui_app_params: ImguiAppParams) -> Tuple[int, int]:
    r = imgui_app_params.app_window_size
    if r[0] == 0 or r[1] == 0:
        r = (150, 150)
    return r


def write_last_run_window_position(win_pos: Tuple[int, int]):
    with open(APP_WINDOW_POS_INI_FILE, "w") as f:
        f.write(f"""
[WIN]
WinPosition={win_pos[0]},{win_pos[1]}
    """)


def last_run_window_position() -> Optional[Tuple[int, int]]:
    if not os.path.isfile(APP_WINDOW_POS_INI_FILE):
        return None
    try:
        with open(APP_WINDOW_POS_INI_FILE, "r") as f:
            lines = f.readlines()

        r = None
        for line in lines:
            if "WinPosition=" in line:
                sizes_str = line[len("WinPosition="):].split(",")
                r = (int(sizes_str[0]), int(sizes_str[1]))
        return r
    except Exception:
        return None

