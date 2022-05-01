from abc import ABC, abstractmethod
from .imgui_app_params import ImguiAppParams
from typing import Tuple, Any, Optional
from imgui.integrations.opengl import BaseOpenGLRenderer


class AnyBackend(ABC):
    imgui_app_params: ImguiAppParams
    gl_context: Any = None
    app_window: Any = None
    gl_renderer : BaseOpenGLRenderer = None

    def __init__(self, imgui_app_params):
        self.imgui_app_params = imgui_app_params

    @abstractmethod
    def init_backend(self, last_run_window_position: Optional[Tuple[int, int]]):
        pass

    @abstractmethod
    def init_gl_renderer(self):
        pass

    @abstractmethod
    def get_screen_size(self) -> Tuple[int, int]:
        pass

    @abstractmethod
    def wait_for_event_power_save(self):
        pass

    @abstractmethod
    def get_window_position(self) -> Tuple[int, int]:
        pass

    @abstractmethod
    def set_window_position(self, window_position: Tuple[int, int]):
        pass

    @abstractmethod
    def get_window_size(self):
        pass

    @abstractmethod
    def set_window_size(self, window_size:Tuple[int, int]):
        pass

    @abstractmethod
    def poll_events(self) -> bool: # Returns true if window was closed
        pass

    @abstractmethod
    def render_gl_draw_data(self):
        pass

    @abstractmethod
    def shutdown_gl_renderer(self):
        pass

    @abstractmethod
    def destroy_window(self):
        pass

    @abstractmethod
    def swap_window(self):
        pass

    @abstractmethod
    def delete_gl_context(self):
        pass

    @abstractmethod
    def destroy_window(self):
        pass

    @abstractmethod
    def quit(self):
        pass

