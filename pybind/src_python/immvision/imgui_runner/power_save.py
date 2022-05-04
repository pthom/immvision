class _PowerSave:
    use_power_save = True
    _idle_wait_time = 1. / 4. # 4 fps if idle
    _max_wait_before_next_frame_seconds: float = _idle_wait_time

    def on_new_frame(self):
        self._max_wait_before_next_frame_seconds = _PowerSave._idle_wait_time

    def power_save_set_max_wait_next_frame(self, time_seconds):
        assert(time_seconds >= 0)
        self._max_wait_before_next_frame_seconds = time_seconds

    def get_event_waiting_timeout(self, window_is_hidden: bool) -> float:
        "Return a time to wait, in *seconds*"
        waiting_time = self._idle_wait_time if window_is_hidden else self._max_wait_before_next_frame_seconds
        return waiting_time


_POWER_SAVE =  _PowerSave()


def _power_save_instance():
    return _POWER_SAVE


"""
#######################################################################################################################
                                    Public API below
#######################################################################################################################
"""

"""
ImGui application can consume a lot of CPU, since they update the screen very frequently (200 FPS is common)
In order to reduce the CPU usage, the screen will not be redrawn if no user interaction
is detected by the backend (i.e Sdl or Glfw).

This is ok most of the time but if you are displaying animated widgets (for example a live video, or an
image that updates continuously), you may want to ask for a faster refresh.

In that case, before the next frame, call: 
      imgui_runner.power_save_set_max_wait_next_frame(time_seconds)

You can also temporarily disable / enable Power Save by calling
    imgui_runner.power_save_disable()
    imgui_runner.power_save_enable()
"""


def power_save_enable():
    _POWER_SAVE.use_power_save = True


def power_save_disable():
    _POWER_SAVE.use_power_save = False


def power_save_set_max_wait_next_frame(time_seconds):
    _POWER_SAVE.power_save_set_max_wait_next_frame(time_seconds)


