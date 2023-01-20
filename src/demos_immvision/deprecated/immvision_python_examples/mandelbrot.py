"""
Calculate Mandelbrot with numba
"""
from typing import Tuple
import numpy as np
from PIL import Image
from numba import njit, jit
import numba
from dataclasses import dataclass
from immvision.debug_utils import timeit
import imgui
import immvision

PreciseFloat = float  # np.float64
ColorType = np.float32


@jit
def lerp(a, b, x):
    return a + (b - a) * x


@njit
def unlerp(a, b, p):
    return (p - a) / (b - a)


@njit(cache=True, nogil=True, parallel=True)
def mandelbrot_numba(
    width: int, height: int, x_center: PreciseFloat, y_center: PreciseFloat, zoom: PreciseFloat, max_iterations: int
) -> np.ndarray:
    result = np.zeros((height, width), ColorType)

    # Compute boundings
    x_width: PreciseFloat = 1.5
    y_height: PreciseFloat = 1.5 * height / width
    x_from = x_center - x_width / zoom
    x_to = x_center + x_width / zoom
    y_from = y_center - y_height / zoom
    y_to = y_center + y_height / zoom

    for iy in numba.prange(height):  # parallel loop! (speedup by a factor of 7 on an 8 cores machines)
        ky: PreciseFloat = iy / height
        y0 = lerp(y_from, y_to, ky)
        for ix in range(width):
            kx: PreciseFloat = ix / width

            # start iteration
            x0 = lerp(x_from, x_to, kx)

            x: PreciseFloat = 0.0
            y: PreciseFloat = 0.0

            # perform Mandelbrot set iterations
            escaped = False
            for iteration in range(max_iterations):
                x_new = x * x - y * y + x0
                y = 2 * x * y + y0
                x = x_new

                # if escaped
                norm2 = x * x + y * y
                if norm2 > 4.0:
                    escaped = True
                    # color using pretty linear gradient
                    color: ColorType = 1.0 - 0.01 * (iteration - np.log2(np.log2(norm2)))
                    break

            if not escaped:
                color = 0.0

            result[iy, ix] = color

    return result


@dataclass
class MandelbrotParams:
    width: int = 800
    height: int = 600
    # x in [-2, 1] and y in [-1, 1]
    x_center: PreciseFloat = -0.5
    y_center: PreciseFloat = 0.0
    zoom: PreciseFloat = 1
    max_iterations: int = 100

    def mandelbrot_image(self):
        return mandelbrot_numba(self.width, self.height, self.x_center, self.y_center, self.zoom, self.max_iterations)

    def set_center(self, x: PreciseFloat, y: PreciseFloat):
        self.x_center = PreciseFloat(x)
        self.y_center = PreciseFloat(y)

    def get_center(self):
        return self.x_center, self.y_center

    def pixel_to_complex_coords(self, pt: immvision.Pixel):
        x_width: PreciseFloat = PreciseFloat(1.5)
        y_height: PreciseFloat = PreciseFloat(1.5) * PreciseFloat(self.height) / PreciseFloat(self.width)
        x_from = self.x_center - x_width / PreciseFloat(self.zoom)
        x_to = self.x_center + x_width / PreciseFloat(self.zoom)
        y_from = self.y_center - y_height / PreciseFloat(self.zoom)
        y_to = self.y_center + y_height / PreciseFloat(self.zoom)

        kx = PreciseFloat(pt[0]) / PreciseFloat(self.width)
        ky = PreciseFloat(pt[1]) / PreciseFloat(self.height)
        real = lerp(x_from, x_to, kx)
        imag = lerp(y_from, y_to, ky)
        return real, imag


@dataclass
class MandelbrotApp:
    mandelbrot_params: MandelbrotParams = MandelbrotParams()
    idx_poi: int = 0
    image: np.ndarray = None
    last_mouse_coords: Tuple[PreciseFloat, PreciseFloat] = None

    # cf https://mandelbrot.dev/
    mandelbrot_poi_list = [
        -0.5,
        0.28439 - 0.01359j,
        0.28443 - 0.01273j,
        -0.79619 - 0.18323j,
        -0.748986 + 0.055768j,
        -0.658448 - 0.466852j,
        -0.715181 - 0.230028j,
        0.148865892 + 0.642407724j,
        -0.596360941 + 0.662749640j,
        -1.99991175020 + 0j,
        -1.67440967428 + 0.00004716557j,
        -0.139975337339 - 0.992076239092j,
        0.432456684114 - 0.340532264460j,
    ]

    def __init__(self):
        self.move_to_poi()
        self.update_image()

    def move_to_poi(self):
        self.mandelbrot_params.x_center = self.mandelbrot_poi_list[self.idx_poi].real
        self.mandelbrot_params.y_center = self.mandelbrot_poi_list[self.idx_poi].imag

    def update_image(self):
        self.image = self.mandelbrot_params.mandelbrot_image()

    def gui(self):
        params = self.mandelbrot_params

        needs_refresh = False

        # Show FPS
        imgui.text(f"FPS: {imgui.get_io().framerate:.1f}")

        # On double click, change the zooming center
        if imgui.is_mouse_double_clicked(0) and self.last_mouse_coords is not None:
            self.mandelbrot_params.set_center(self.last_mouse_coords[0], self.last_mouse_coords[1])
            needs_refresh = True

        # Change location to interesting location
        changed, self.idx_poi = imgui.slider_int(
            "interesting location", self.idx_poi, 0, len(self.mandelbrot_poi_list) - 1
        )
        if changed:
            needs_refresh = True
            self.move_to_poi()

        # Edit zoom
        logarithmic_flag = (1 << 4) | (1 << 5)
        changed, new_zoom = imgui.slider_float("zoom", params.zoom, 0.5, 1e12, format="%.4G", flags=logarithmic_flag)
        params.zoom = PreciseFloat(new_zoom)
        if changed:
            needs_refresh = True

        center = self.mandelbrot_params.get_center()
        imgui.text(f"center: {center[0]} + {center[1]} * i")

        # Edit max_iterations
        changed, params.max_iterations = imgui.slider_int("iterations", params.max_iterations, 1, 1000)
        if changed:
            needs_refresh = True

        # Edit image size
        imgui.set_next_item_width(200)
        changed, params.width = imgui.slider_int("Width", params.width, 400, 2000)
        if changed:
            needs_refresh = True
        imgui.same_line()
        imgui.set_next_item_width(200)
        changed, params.height = imgui.slider_int("Height", params.height, 400, 2000)
        if changed:
            needs_refresh = True

        # Edit position
        changed, params.x_center = imgui.slider_float("x", params.x_center, -2.0, 1.0, "%.6f")
        if changed:
            needs_refresh = True
        changed, params.y_center = imgui.slider_float("y", params.y_center, -1.0, 1.0, "%.6f")
        if changed:
            needs_refresh = True

        # recalculate image if needed
        # self.image_params.refresh_image = needs_refresh
        if needs_refresh:
            self.update_image()
            print("needs_refresh")

        mouse_position = immvision.image_display(
            "mandelbrot", self.image, refresh_image=needs_refresh, show_options_button=True
        )

        if mouse_position[0] >= 0.0:
            complex_coords = self.mandelbrot_params.pixel_to_complex_coords(mouse_position)
            imgui.text(f"Complex position: {complex_coords[0]} + {complex_coords[1]} * i")
            self.last_mouse_coords = complex_coords
        else:
            self.last_mouse_coords = None

    def run(self):
        # immvision.power_save_disable()
        immvision.run(lambda: self.gui())


def playground0():
    # Try mandelbrot_numba standalone
    params = MandelbrotParams()
    mandel_image = params.mandelbrot_image()

    # convert from float in [0, 1] to to uint8 in [0, 255] for PIL
    mandel_image = np.clip(mandel_image * 255, 0, 255).astype(np.uint8)
    mandel_image = Image.fromarray(mandel_image)
    mandel_image.show()


def playground():
    app = MandelbrotApp()
    app.run()


def measure_perf():
    params = MandelbrotParams()

    @timeit(nb_calls=100, print_all_times=False, numba_skip_first_compil=True)
    def truc():
        m = params.mandelbrot_image()

    truc()


if __name__ == "__main__":
    playground()
    # measure_perf()
