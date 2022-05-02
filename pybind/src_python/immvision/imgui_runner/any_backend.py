from abc import ABC, abstractmethod
from .imgui_app_params import _ImguiAppParamsHelper, ImguiAppParams
from typing import Tuple, Any, Optional
from imgui.integrations.opengl import BaseOpenGLRenderer
from .backend_types import WindowPosition, WindowSize, WindowBounds


class AnyBackend(ABC):
    imgui_app_params_helper: _ImguiAppParamsHelper
    imgui_app_params: ImguiAppParams
    gl_context: Any = None
    app_window: Any = None
    imgui_gl_renderer : BaseOpenGLRenderer = None

    def __init__(self, imgui_app_params_helper):
        self.imgui_app_params_helper = imgui_app_params_helper
        self.imgui_app_params = self.imgui_app_params_helper.imgui_app_params

    @abstractmethod
    def init_backend(self):
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
    def get_monitor_work_area_from_index(self, monitor_index: int) -> WindowBounds:
        """The area of a monitor not occupied by global task bars or menu bars is the work area.
        This is specified in screen coordinates.
        Pixels and screen coordinates may map 1:1 on your machine, but they won't on every other machine,
        for example on a Mac with a Retina display.
        """
        pass

    @abstractmethod
    def get_monitor_index_from_window_position(self, window_position: WindowPosition) -> int:
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
        self.set_window_size(window_bounds.window_size)
        self.set_window_position(window_bounds.window_position)

    def initial_window_position(self, window_bounds: WindowBounds):
        if window_bounds.window_position is not None:
            return window_bounds.window_position

        # if no position given, place the app at the center of the selected monitor
        window_size = window_bounds.window_size
        assert self.imgui_app_params.app_window_position_monitor_index >= 0
        assert self.imgui_app_params.app_window_position_monitor_index < self.get_nb_monitors()
        monitor_work_area = self.get_monitor_work_area_from_index(self.imgui_app_params.app_window_position_monitor_index)
        monitor_center = monitor_work_area.center()
        window_position = (monitor_center[0] - int(window_size[0] / 2), monitor_center[1] - int(window_size[1] / 2))
        return window_position

    def get_current_monitor_work_area(self) -> int:
        monitor_index = self.get_monitor_index_from_window_position(self.get_window_position())
        return self.get_monitor_work_area_from_index(monitor_index)
