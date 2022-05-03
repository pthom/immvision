from __future__ import annotations
from typing import Tuple, Callable
from dataclasses import dataclass

# GuiFunction: any function that display widgets using for example imgui.text(), imgui.button(), etc
# it should take no parameter and return nothing (use lambdas if you need to capture the global app state)
GuiFunction = Callable[[], None]

Float_Between_0_1 = float
Color_RGBA_Float = Tuple[Float_Between_0_1, Float_Between_0_1, Float_Between_0_1, Float_Between_0_1]

WindowPosition = Tuple[int, int]
WindowSize = Tuple[int, int]
Pixel = Tuple[int, int]


@dataclass
class WindowBounds:
    window_position: WindowPosition = None
    window_size: WindowSize = None

    def top_left_corner(self) -> Pixel:
        return self.window_position

    def bottom_right_corner(self) -> Pixel:
        return self.window_position[0] + self.window_size[0], self.window_position[1] + self.window_size[1]

    def center(self) -> Pixel:
        r = (self.window_position[0] + int(self.window_size[0] / 2),
             self.window_position[1] + int(self.window_size[1] / 2))
        return r

    def contains(self, pixel: Pixel) -> bool:
        for dim in range(2):
            if pixel[dim] < self.top_left_corner()[dim]:
                return False
            if pixel[dim] < self.top_left_corner()[dim]:
                return False
        return True

    def win_position_centered(self, window_size: WindowSize) -> Pixel:
        r = ( self.center()[0] - int(window_size[0] / 2),
              self.center()[1] - int(window_size[1]/ 2) )
        return r

    def distance_from_pixel(self, point: Pixel):
        def dist_from_interval(a, b, x):
            if x < a:
                return a - x
            elif x > b:
                return x - b
            else:
                return 0
        distance = 0
        for dim in range(2):
            distance += dist_from_interval(self.top_left_corner()[dim], self.bottom_right_corner()[dim], point[dim])
        return distance

    def ensure_window_fits_this_monitor(self, window_bounds_original: WindowBounds) -> WindowBounds:
        def convert_list_to_tuple(v):
            assert len(v) == 2
            return v[0], v[1]

        def convert_tuple_to_list(v):
            assert len(v) == 2
            return [v[0], v[1]]

        def convert_bounds_to_list(v: WindowBounds) -> WindowBounds:
            return WindowBounds(
                convert_tuple_to_list(v.window_position),
                convert_tuple_to_list(v.window_size))

        def convert_bounds_to_tuple(v: WindowBounds) -> WindowBounds:
            return WindowBounds(
                convert_list_to_tuple(v.window_position),
                convert_list_to_tuple(v.window_size))

        # transform tuples into lists to make them mutable
        window_bounds_new = convert_bounds_to_list(window_bounds_original)

        for dim in range(2):
            # if window is to the left or to the top, move it
            if window_bounds_new.window_position[dim] < self.window_position[dim]:
                window_bounds_new.window_position[dim] = self.window_position[dim]
            # if the window is too big and does not fit the bottom right corner, try to move it
            if window_bounds_new.bottom_right_corner()[dim] >= self.bottom_right_corner()[dim]:
                window_bounds_new.window_position[dim] = \
                    self.bottom_right_corner()[dim] - 1 - window_bounds_new.window_size[dim]
            # if it was not enough, resize it
            if window_bounds_new.bottom_right_corner()[dim] >= self.bottom_right_corner()[dim]:
                window_bounds_new.window_size[dim] = self.window_size[dim]

        window_bounds_new = convert_bounds_to_tuple(window_bounds_new)
        return window_bounds_new



def __eq__(self, other):
        for dim in range(2):
            if self.window_size[dim] != other.window_size[dim]:
                return False
            if self.window_position[dim] != other.window_position[dim]:
                return False
        return True
