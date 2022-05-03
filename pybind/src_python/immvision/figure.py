import matplotlib
import matplotlib.figure
import numpy
from typing import Dict, Tuple
import cv2
from . import ImageParams, image, image_display
from .cv_types import *


_Id = int
_Image = numpy.ndarray

_FIG_CACHE: Dict[_Id, _Image] = {}
_DEFAULT_BG_COLOR: Color_RGBA_Int = (230, 230, 230, 255)


def _fig_to_array(figure: matplotlib.figure.Figure, refresh: bool, bg_color: Color_RGBA_Int):
    fig_id = id(figure)
    if fig_id not in _FIG_CACHE or refresh:
        # draw the renderer
        figure.canvas.draw()
        # Get the RGBA buffer from the figure
        buffer_rgba = figure.canvas.buffer_rgba()
        buf = numpy.frombuffer(buffer_rgba, dtype=numpy.uint8)
        buf.shape = buffer_rgba.shape
        img_rgba = buf
        transparent_zones = img_rgba[ :, : , 3] == 0
        img_rgba[transparent_zones] = bg_color
        _FIG_CACHE[fig_id] = img_rgba

        matplotlib.pyplot.close(figure)

    return _FIG_CACHE[fig_id]


def figure(figure: matplotlib.figure.Figure, params: ImageParams, bg_color = _DEFAULT_BG_COLOR):
    """
    imgui_fig.fig will display a matplotlib figure

    Note: this might fail on OSX, with the following message ::

        AttributeError: 'FigureCanvasMac' object has no attribute 'renderer'

    In this case, simply change the renderer to Tk, like this::

        import matplotlib
        matplotlib.use('TkAgg')  # this has to be done *before* importing pyplot
        import matplotlib.pyplot
    """
    m = _fig_to_array(figure, params.refresh_image, bg_color)
    params.is_color_order_bgr = False
    image(m, params)


def figure_display(
        figure: matplotlib.figure.Figure,
        image_display_size:  Size  = (0, 0),
        refresh_image: bool  = False,
        show_options: bool  = True,
        bg_color = _DEFAULT_BG_COLOR
    ):
    m = _fig_to_array(figure, refresh_image, bg_color)
    is_bgr = False
    image_display(m, image_display_size, refresh_image, show_options, is_bgr)
