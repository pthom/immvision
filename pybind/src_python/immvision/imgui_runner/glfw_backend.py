from .any_backend import AnyBackend
import glfw
from typing import Optional, Tuple, Any
from .power_save import power_save_instance
from imgui.integrations.glfw import GlfwRenderer
from .gui_types import WindowPosition, WindowSize, WindowBounds
from .imgui_app_params import ImguiAppParams, _ImguiAppParamsHelper
import OpenGL.GL as gl


class GlfwBackend(AnyBackend):
    def __init__(self, imgui_app_params_helper: _ImguiAppParamsHelper):
        AnyBackend.__init__(self, imgui_app_params_helper)

    def get_nb_monitors(self):
        monitors = glfw.get_monitors()
        assert len(monitors) > 0
        return len(monitors)

    def get_monitor_work_area_from_index(self, monitor_index: int) -> WindowBounds:
        monitors = glfw.get_monitors()
        assert monitor_index >= 0
        assert monitor_index < len(monitors)
        x, y, w, h = glfw.get_monitor_workarea(monitors[monitor_index])
        return WindowBounds((x, y), (w, h))

    def init_backend(self):
        if not glfw.init():
            print("Could not initialize OpenGL context")
            exit(1)

        # OS X supports only forward-compatible core profiles from 3.2
        glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 3)
        glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 3)
        glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)

        glfw.window_hint(glfw.OPENGL_FORWARD_COMPAT, gl.GL_TRUE)

        window_bounds = self.initial_window_bounds()

        # Create a windowed mode window and its OpenGL context
        no_window_shared_resources = None
        no_full_screen_monitor = None
        self.app_window = glfw.create_window(
            window_bounds.window_size[0],
            window_bounds.window_size[1],
            self.imgui_app_params.app_window_title,
            no_full_screen_monitor,
            no_window_shared_resources
        )

        glfw.make_context_current(self.app_window)

        if not self.app_window:
            glfw.terminate()
            raise RuntimeError("Glfw: Could not initialize Window")

        if window_bounds.window_position is not None:
            self.set_window_position(window_bounds.window_position)

        self.raise_window()

    def init_imgui_gl_renderer(self):
        self.imgui_gl_renderer = GlfwRenderer(self.app_window)

    def destroy_imgui_gl_renderer(self):
        self.imgui_gl_renderer.shutdown()

    def wait_for_event_power_save(self):
        waiting_time = power_save_instance().get_event_waiting_timeout(self.is_window_hidden())
        if waiting_time > 0:
            glfw.wait_events_timeout(waiting_time)

    def get_window_position(self) -> WindowPosition:
        return glfw.get_window_pos(self.app_window)

    def set_window_position(self, window_position: WindowPosition):
        glfw.set_window_pos(self.app_window, window_position[0], window_position[1])

    def get_window_size(self):
        return glfw.get_window_size(self.app_window)

    def set_window_size(self, window_size: WindowSize):
        glfw.set_window_size(self.app_window, window_size[0], window_size[1])

    def poll_events(self):
        glfw.poll_events()
        self.imgui_gl_renderer.process_inputs()

    def should_window_close(self) -> bool:
        return glfw.window_should_close(self.app_window)

    def render_gl_draw_data(self, draw_data):
        self.imgui_gl_renderer.render(draw_data)

    def destroy_imgui_gl_renderer(self):
        self.imgui_gl_renderer.shutdown()

    def destroy_window(self):
        # glfw: destroy_window is a No-Op
        pass

    def destroy_gl_context(self):
        # glfw: destroy_gl_context is a No-Op
        pass

    def swap_window(self):
        glfw.swap_buffers(self.app_window)

    def is_window_hidden(self) -> bool:
        r = glfw.get_window_attrib(self.app_window, glfw.VISIBLE)
        return r > 0

    def quit(self):
        glfw.terminate()

    def raise_window(self):
        glfw.show_window(self.app_window)
        glfw.focus_window(self.app_window)
        glfw.request_window_attention(self.app_window)
