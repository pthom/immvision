class _PowerSave:
    use_power_save = True
    _idle_wait_time = 1. / 4. # 4 fps if idle
    _max_wait_before_next_frame_seconds: float = _idle_wait_time

    def on_new_frame(self):
        self._max_wait_before_next_frame_seconds = _PowerSave._idle_wait_time

    def set_max_wait_before_next_frame(self, time_seconds):
        assert(time_seconds >= 0)
        self._max_wait_before_next_frame_seconds = time_seconds

    def get_event_waiting_timeout(self, window_is_hidden: bool) -> float:
        "Return a time to wait, in *seconds*"
        waiting_time = self._idle_wait_time if window_is_hidden else self._max_wait_before_next_frame_seconds
        return waiting_time


_POWER_SAVE =  _PowerSave()


def power_save_instance():
    return _POWER_SAVE


def set_max_wait_before_next_frame(time_seconds):
    _POWER_SAVE.set_max_wait_before_next_frame(time_seconds)
