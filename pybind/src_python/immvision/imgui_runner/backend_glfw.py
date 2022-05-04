import logging

from .backend_any import BackendAny
import glfw
from .power_save import _power_save_instance
from imgui.integrations.glfw import GlfwRenderer
from .gui_types import WindowPosition, WindowSize, WindowBounds
import OpenGL.GL as gl
import time
from . import _DEBUG_IMGUI_RUNNER
from .debug_utils import log


if _DEBUG_IMGUI_RUNNER:
    from .debug_utils import verbose_function
    glfw.set_window_pos = verbose_function(dump_args=True, dump_args_at_exit=False, dump_return=True)(glfw.set_window_pos)
    glfw.create_window = verbose_function(dump_args=True, dump_args_at_exit=False, dump_return=True)(glfw.create_window)
    glfw.set_window_size = verbose_function(dump_args=True, dump_args_at_exit=False, dump_return=True)(glfw.set_window_size)


class BackendGlfw(BackendAny):
    def __init__(self):
        BackendAny.__init__(self)

    def get_nb_monitors(self):
        monitors = glfw.get_monitors()
        assert len(monitors) > 0
        return len(monitors)

    def get_one_monitor_work_area(self, monitor_index: int) -> WindowBounds:
        monitors = glfw.get_monitors()
        assert monitor_index >= 0
        assert monitor_index < len(monitors)
        x, y, w, h = glfw.get_monitor_workarea(monitors[monitor_index])
        return WindowBounds((x, y), (w, h))

    def init_backend(self):
        if not glfw.init():
            log(logging.ERROR, "glfw.init failed!")

    def init_backend_window(
            self,
            window_title: str,
            window_bounds: WindowBounds,
            fullscreen: bool,
            fullscreen_monitor_idx: int,
            gl_multisamples: int
        ):
        self.full_screen = fullscreen
        self.full_screen_monitor_idx = fullscreen_monitor_idx

        # OS X supports only forward-compatible core profiles from 3.2
        glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 3)
        glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 3)
        glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)

        glfw.window_hint(glfw.OPENGL_FORWARD_COMPAT, gl.GL_TRUE)

        if fullscreen:
            # Create a full screen window
            nb_monitors = self.get_nb_monitors()
            assert 0 <= fullscreen_monitor_idx < nb_monitors
            monitor = glfw.get_monitors()[fullscreen_monitor_idx]
            video_mode = glfw.get_video_mode(monitor)
            glfw.window_hint(glfw.RED_BITS, video_mode.bits.red)
            glfw.window_hint(glfw.GREEN_BITS, video_mode.bits.green)
            glfw.window_hint(glfw.BLUE_BITS, video_mode.bits.blue)
            glfw.window_hint(glfw.REFRESH_RATE, video_mode.refresh_rate)
            if window_bounds.window_size is None:
                video_size = (video_mode.size.width, video_mode.size.height)
            else:
                video_size = window_bounds.window_size
            self.app_window = glfw.create_window(
                video_size[0], video_size[1],
                window_title,
                monitor,
                None
            )

        else:
            # Create a windowed mode window
            no_window_shared_resources = None
            no_full_screen_monitor = None
            self.app_window = glfw.create_window(
                window_bounds.window_size[0],
                window_bounds.window_size[1],
                window_title,
                no_full_screen_monitor,
                no_window_shared_resources
            )

        glfw.make_context_current(self.app_window)

        if not self.app_window:
            glfw.terminate()
            log(logging.ERROR, "Glfw: Could not initialize Window")

        if window_bounds.window_position is not None:
            self.set_window_position(window_bounds.window_position)

        self.raise_window()

    def init_imgui_gl_renderer(self):
        self.imgui_gl_renderer = GlfwRenderer(self.app_window)

    def destroy_imgui_gl_renderer(self):
        self.imgui_gl_renderer.shutdown()

    def wait_for_event_power_save(self):
        waiting_time = _power_save_instance().get_event_waiting_timeout(self.is_window_hidden())
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
        glfw.hide_window(self.app_window)
        time.sleep(0.3)
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
