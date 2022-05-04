import cv2
import matplotlib
import matplotlib.figure
import numpy
from typing import Dict, Tuple
from . import ImageParams, image, image_display
from .cv_types import *


_Id = int
_Image = numpy.ndarray

_FIG_CACHE: Dict[_Id, _Image] = {}
_DEFAULT_BG_COLOR: Color_RGBA_Int = (230, 230, 230, 255)


def nparray_address(m: numpy.ndarray):
    return hex(m.__array_interface__['data'][0])


def _fig_to_array(fig: matplotlib.figure.Figure, refresh: bool, bg_color: Color_RGBA_Int) -> _Image:
    fig_id = id(fig)
    if fig_id not in _FIG_CACHE or refresh:
        fig.canvas.draw()

        buffer_rgba = fig.canvas.buffer_rgba()
        buf = numpy.frombuffer(buffer_rgba, dtype=numpy.uint8)
        buf.shape = buffer_rgba.shape
        img_rgba = buf.copy()

        matplotlib.pyplot.close(fig)

        transparent_zones = img_rgba[ :, : , 3] == 0
        img_rgba[transparent_zones] = bg_color
        _FIG_CACHE[fig_id] = img_rgba

    return _FIG_CACHE[fig_id]



def figure(label_id: str, fig: matplotlib.figure.Figure, params: ImageParams, bg_color = _DEFAULT_BG_COLOR):
    """
    Display a matplotlib figure with options to navigate the figure as a normal image.
    Most of the time, you will prefer to use figure_display, which displays the figure alone.

    :param label_id:
        The label to display. Should be unique. If you do not want the label to show, use a string like
        "##some_unique_id". Everything after "##" is hidden
    :param fig: the figure to display
    :param params: see ImageParams definition
    :param bg_color: many figures have a transparent border. You can change the color of this border
    """
    m = _fig_to_array(fig, params.refresh_image, bg_color)
    params.is_color_order_bgr = False
    image(label_id, m, params)


def figure_display(
        label_id: str,
        fig: matplotlib.figure.Figure,
        image_display_size:  Size  = (0, 0),
        refresh_image: bool  = False,
        show_options_button: bool  = False,
        bg_color = _DEFAULT_BG_COLOR
    ):
    """
    immvision.figure_display will display a matplotlib figure

    :param label_id:
        The label to display. Should be unique. If you do not want the label to show, use a string like
        "##some_unique_id". Everything after "##" is hidden
    :param fig: the figure to display
        If you created the figure with plt.subplots (fig, ax = plt.subplots(...)) then pass fig as an argument
        If you created the figure with plt.imshow, use this:  ax = plt.imshow(...); immvision.figure_display(ax.figure)
    :param image_display_size: displayed size
    :param refresh_image: figures are cached. Set to true if you updated the figure and it is not updated on the screen
    :param show_options_button: if True, display an "option" button
    :param bg_color: many figures have a transparent border. You can change the color of this border

    Note: This might fail on OSX, with the following message ::
            "AttributeError: 'FigureCanvasMac' object has no attribute 'renderer'"
          In this case, simply change the renderer to Tk, like this::
            import matplotlib
            matplotlib.use('TkAgg')  # this has to be done *before* importing pyplot
            import matplotlib.pyplot
    """
    m = _fig_to_array(fig, refresh_image, bg_color)
    is_bgr = False
    # print(f"{nparray_address(m)} => Python image_display size={m.shape[1]}, {m.shape[0]}")
    image_display(label_id, m, image_display_size, refresh_image, show_options_button, is_bgr)
