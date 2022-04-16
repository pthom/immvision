#!/usr/bin/env python3
import OpenGL.GL as gl
import cv2
import numpy as np
import imgui
import typing

COUNTER_Blit_RGBA_Buffer = 0

def _Blit_RGBA_Buffer(img_rgba: np.ndarray, texture_id: int) -> None:
    global COUNTER_Blit_RGBA_Buffer
    COUNTER_Blit_RGBA_Buffer += 1
    print(f"py Blit_RGBA_Buffer {COUNTER_Blit_RGBA_Buffer=}")
    width = img_rgba.shape[1]
    height = img_rgba.shape[0]
    gl.glPixelStorei(gl.GL_UNPACK_ALIGNMENT, 1)
    gl.glBindTexture(gl.GL_TEXTURE_2D, texture_id)
    gl.glPixelStorei(gl.GL_UNPACK_ALIGNMENT, 1)
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MAG_FILTER, gl.GL_LINEAR)
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MIN_FILTER, gl.GL_LINEAR)
    gl.glTexImage2D(
        gl.GL_TEXTURE_2D,
        0,
        gl.GL_RGBA,
        width,
        height,
        0,
        gl.GL_RGBA,
        gl.GL_UNSIGNED_BYTE,
        img_rgba,
    )
    gl.glBindTexture(gl.GL_TEXTURE_2D, 0)



def _GenTexture() -> int:
    try:
        texture_id = gl.glGenTextures(1)
    except Exception as e:
        print(f"_gl_provider_python.py: glGenTextures failed : {e}")
    return texture_id


def _DeleteTexture(texture_id: int):
    try:
        gl.glDeleteTextures([texture_id])
    except Exception as e:
        print(f"_gl_provider_python.py: glDeleteTextures failed : {e}")


def _image(
        texture_id: int,
        float_width: float,
        float_height: float,
        tuple_uv0: typing.Tuple[float, float]= (0., 0.),
        tuple_uv1: typing.Tuple[float, float]= (1., 1.),
        tuple_tint_color: typing.Tuple[float, float, float, float] =(1., 1., 1., 1.),
        tuple_border_color: typing.Tuple[float, float, float, float] =(0., 0., 0., 0.)
          ):
    imgui.image(texture_id, float_width, float_height, tuple_uv0, tuple_uv1, tuple_tint_color, tuple_border_color)


def _image_button(
        texture_id: int,
        float_width: float,
        float_height: float,
        tuple_uv0: typing.Tuple[float, float]= (0., 0.),
        tuple_uv1: typing.Tuple[float, float]= (1., 1.),
        tuple_tint_color: typing.Tuple[float, float, float, float] =(1., 1., 1., 1.),
        tuple_border_color: typing.Tuple[float, float, float, float] =(0., 0., 0., 0.),
        int_frame_padding: int = -1
        ):
    imgui.image_button(texture_id, float_width, float_height, tuple_uv0, tuple_uv1, tuple_tint_color, tuple_border_color, int_frame_padding)


def _get_window_draw_list_add_image(
        texture_id,
        p_min: typing.Tuple[float, float],  # 2 values (top left corner)
        p_max: typing.Tuple[float, float],  # 2 values (bottom right corner)
        uv_a: typing.Tuple[float, float] = (0,0),
        uv_b: typing.Tuple[float, float] = (1,1),
        col: int = 0xffffffff
        ):
    imgui.get_window_draw_list().add_image(texture_id, p_min, p_max, uv_a, uv_b, col)
