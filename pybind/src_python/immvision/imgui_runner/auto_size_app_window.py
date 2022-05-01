from .imgui_app_params import ImguiAppParams
import imgui
import os
from typing import Tuple, Optional
from .any_backend import AnyBackend


class AutoSizeAppWindow:
    """
    This class can resize the app window to the size of the imgui widgets seen on the first frame
    """
    _imgui_app_params: ImguiAppParams = None
    _last_seen_imgui_window_size : Optional[Tuple[int, int]]= None
    _computed_window_size: Optional[Tuple[int, int]] = None
    _backend: AnyBackend = None
    _flag_is_measuring_size: bool = False

    def __init__(self, imgui_app_params: ImguiAppParams, backend: AnyBackend):
        self._imgui_app_params = imgui_app_params
        self._backend = backend

    def _want_autosize(self):
        return self._imgui_app_params.app_window_size[0] == 0 or self._imgui_app_params.app_window_size[1] == 0

    def _move_window_if_out_of_screen_bounds(self):
        win_pos = list(self._backend.get_window_position())
        win_size = self._backend.get_window_size()
        screen_size = self._backend.get_screen_size()

        for dim in range(2):
            if win_pos[dim] < 0:
                win_pos[dim] = 0
            if win_pos[dim] + win_size[dim] > screen_size[dim]:
                win_pos[dim] = screen_size[dim] - win_size[dim]
                if win_pos[dim] < 0:
                    win_pos[dim] = 0

        self._backend.set_window_position(win_pos)

    def _force_window_size(self):
        user_widgets_size = imgui.get_item_rect_size()
        widgets_margin = 30
        screen_size = self._backend.get_screen_size()

        self._computed_window_size = (
            min(int(user_widgets_size[0]) + widgets_margin, screen_size[0]),
            min(int(user_widgets_size[1]) + widgets_margin, screen_size[0]))

        self._backend.set_window_size(self._computed_window_size)
        self._move_window_if_out_of_screen_bounds()


    @staticmethod
    def _begin_full_size_imgui_window():
        imgui.set_next_window_position(0, 0)
        view_port = imgui.get_main_viewport()
        imgui.set_next_window_position(view_port.pos.x, view_port.pos.y)
        viewport_size = (float(view_port.size[0]), float(view_port.size[1]))
        imgui.set_next_window_size(viewport_size[0], viewport_size[1])
        imgui.begin("Default window", False, imgui.WINDOW_NO_TITLE_BAR | imgui.WINDOW_HORIZONTAL_SCROLLING_BAR)

    def begin(self):
        """
        Reproduces imgui.begin behavior for an imgui window that occupies the whole app window
        and that will try to resize the SDL app window to its content size
        """
        AutoSizeAppWindow._begin_full_size_imgui_window()
        if self._want_autosize() and self._computed_window_size is None:
            self._flag_is_measuring_size = True
            imgui.begin_group()
        else:
            self._flag_is_measuring_size = False

    def end(self):
        if self._flag_is_measuring_size:
            imgui.end_group()
            self._force_window_size()
        imgui.end()
