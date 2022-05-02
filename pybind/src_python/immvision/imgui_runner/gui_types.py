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
