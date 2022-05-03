from abc import ABC, abstractmethod
from typing import Any, List, Optional
from imgui.integrations.opengl import BaseOpenGLRenderer
from .gui_types import WindowPosition, WindowSize, WindowBounds


class BackendAny(ABC):
    gl_context: Any = None
    app_window: Any = None
    imgui_gl_renderer : BaseOpenGLRenderer = None
    full_screen: Optional[bool] = None
    full_screen_monitor_idx: Optional[int] = None

    def __init__(self):
        pass

    @abstractmethod
    def init_backend(self):
        """
        Just init the backend, do not create a window
        """
        pass

    @abstractmethod
    def init_backend_window(
            self,
            window_title: str,
            window_bounds: WindowBounds,
            fullscreen: bool,
            fullscreen_monitor_idx: int,
            gl_multisamples: int
        ):
        """Should set self.fullscreen and self.full_screen_monitor_idx"""
        pass

    @abstractmethod
    def init_imgui_gl_renderer(self):
        pass

    @abstractmethod
    def destroy_imgui_gl_renderer(self):
        pass

    @abstractmethod
    def get_nb_monitors(self):
        pass

    @abstractmethod
    def get_one_monitor_work_area(self, monitor_index: int) -> WindowBounds:
        """The area of a monitor not occupied by global task bars or menu bars is the work area.
        This is specified in screen coordinates.
        Pixels and screen coordinates may map 1:1 on your machine, but they won't on every other machine,
        for example on a Mac with a Retina display.
        """
        pass

    @abstractmethod
    def is_window_hidden(self) -> bool:
        pass

    @abstractmethod
    def wait_for_event_power_save(self):
        pass

    @abstractmethod
    def get_window_position(self) -> WindowPosition:
        pass

    @abstractmethod
    def set_window_position(self, window_position: WindowPosition):
        pass

    @abstractmethod
    def get_window_size(self) -> WindowSize:
        pass

    @abstractmethod
    def set_window_size(self, window_size: WindowSize):
        pass

    @abstractmethod
    def poll_events(self):
        pass

    @abstractmethod
    def should_window_close(self) -> bool:
        pass

    @abstractmethod
    def render_gl_draw_data(self):
        pass

    @abstractmethod
    def destroy_window(self):
        pass

    @abstractmethod
    def swap_window(self):
        pass

    @abstractmethod
    def destroy_window(self):
        pass

    @abstractmethod
    def destroy_gl_context(self):
        pass

    @abstractmethod
    def quit(self):
        pass

    @abstractmethod
    def raise_window(self):
        pass

    def get_window_bounds(self) -> WindowBounds:
        return WindowBounds(self.get_window_position(), self.get_window_size())

    def set_window_bounds(self, window_bounds: WindowBounds):
        current_window_bounds = self.get_window_bounds()
        if current_window_bounds == window_bounds:
            return
        self.set_window_size(window_bounds.window_size)
        self.set_window_position(window_bounds.window_position)

    def get_current_monitor_work_area(self) -> WindowBounds:
        monitor_index = self.get_monitor_index_from_window_position(self.get_window_position())
        return self.get_one_monitor_work_area(monitor_index)

    def get_monitor_index_from_window_position(self, window_position: WindowPosition) -> int:
        if self.full_screen:
            return self.full_screen_monitor_idx
        for monitor_index in range(self.get_nb_monitors()):
            monitor_work_area = self.get_one_monitor_work_area(monitor_index)
            if monitor_work_area.contains(window_position):
                return monitor_index

        #
        # Handle failure and lost windows:
        # if we did not find any monitor containing the window position, return the closest monitor
        #
        distances = []
        for monitor_index in range(self.get_nb_monitors()):
            distances.append(self.get_one_monitor_work_area(monitor_index).distance_from_pixel(window_position))
        min_distance = min(distances)
        best_monitor_idx = distances.index(min_distance)
        return best_monitor_idx

    def move_window_if_out_of_screen_bounds(self):
        current_monitor_work_area = self.get_current_monitor_work_area()
        current_window_bounds = self.get_window_bounds()
        window_bounds_new = current_monitor_work_area.ensure_window_fits_this_monitor(current_window_bounds)
        if window_bounds_new != current_window_bounds:
            self.set_window_bounds(window_bounds_new)

    def get_all_monitors_work_areas(self) -> List[WindowBounds]:
        r = []
        for i in range(self.get_nb_monitors()):
            r.append(self.get_one_monitor_work_area(i))
        return r
