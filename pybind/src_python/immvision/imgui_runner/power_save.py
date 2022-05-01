class _PowerSave:
    use_power_save = True
    _idle_wait_time = 1. / 4. # 4 fps if idle
    _max_wait_before_next_frame_seconds: float = _idle_wait_time

    def on_new_frame(self):
        self._max_wait_before_next_frame_seconds = _PowerSave._idle_wait_time

    def set_max_wait_before_next_frame(self, time_seconds):
        assert(time_seconds >= 0)
        self._max_wait_before_next_frame_seconds = time_seconds

    def get_event_waiting_time(self):
        return self._max_wait_before_next_frame_seconds


_POWER_SAVE =  _PowerSave()


def power_save_instance():
    return _POWER_SAVE


def set_max_wait_before_next_frame(time_seconds):
    _POWER_SAVE.set_max_wait_before_next_frame(time_seconds)
