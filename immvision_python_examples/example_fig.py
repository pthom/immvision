import numpy as np
import matplotlib.pyplot as plt
from matplotlib import patheffects

import imgui
import immvision


def mandelbrot(height, width, x=-0.5, y=0, zoom=1, max_iterations=100):
    # To make navigation easier we calculate these values
    x_width = 1.5
    y_height = 1.5 * height / width
    x_from = x - x_width / zoom
    x_to = x + x_width / zoom
    y_from = y - y_height / zoom
    y_to = y + y_height / zoom
    # Here the actual algorithm starts
    x = np.linspace(x_from, x_to, width).reshape((1, width))
    y = np.linspace(y_from, y_to, height).reshape((height, 1))
    c = x + 1j * y
    # Initialize z to all zero
    z = np.zeros(c.shape, dtype=np.complex128)
    # To keep track in which iteration the point diverged
    div_time = np.zeros(z.shape, dtype=int)
    # To keep track on which points did not converge so far
    m = np.full(c.shape, True, dtype=bool)
    for i in range(max_iterations):
        z[m] = z[m] ** 2 + c[m]
        diverged = np.greater(np.abs(z), 2, out=np.full(c.shape, False), where=m)  # Find diverging
        div_time[diverged] = i  # set the value of the diverged iteration number
        # m[np.abs(z) > 2] = False    # to remember which have diverged
        m[diverged] = False
    return div_time


def mandelbrot_figure():
    ax = plt.imshow(mandelbrot(800, 1000), cmap="magma")
    return ax


def make_figure_classic():
    fig, ax = plt.subplots(figsize=(6, 6))

    nx = 101
    ny = 105

    # Set up survey vectors
    xvec = np.linspace(0.001, 4.0, nx)
    yvec = np.linspace(0.001, 4.0, ny)

    # Set up survey matrices.  Design disk loading and gear ratio.
    x1, x2 = np.meshgrid(xvec, yvec)

    # Evaluate some stuff to plot
    obj = x1**2 + x2**2 - 2 * x1 - 2 * x2 + 2
    g1 = -(3 * x1 + x2 - 5.5)
    g2 = -(x1 + 2 * x2 - 4.5)
    g3 = 0.8 + x1**-3 - x2

    cntr = ax.contour(x1, x2, obj, [0.01, 0.1, 0.5, 1, 2, 4, 8, 16], colors="black")
    ax.clabel(cntr, fmt="%2.1f", use_clabeltext=True)

    cg1 = ax.contour(x1, x2, g1, [0], colors="sandybrown")
    plt.setp(cg1.collections, path_effects=[patheffects.withTickedStroke(angle=135)])

    cg2 = ax.contour(x1, x2, g2, [0], colors="orangered")
    plt.setp(cg2.collections, path_effects=[patheffects.withTickedStroke(angle=60, length=2)])

    cg3 = ax.contour(x1, x2, g3, [0], colors="mediumblue")
    plt.setp(cg3.collections, path_effects=[patheffects.withTickedStroke(spacing=7)])

    ax.set_xlim(0, 4)
    ax.set_ylim(0, 4)

    # plt.show()
    return fig


mandelbrot_ax = mandelbrot_figure()
fig_classic = make_figure_classic()


def gui1():
    immvision.figure_display("mandelbrot_ax", mandelbrot_ax.figure, (300, 0))
    if imgui.button("Exit"):
        immvision.exit_app()


def gui2():
    immvision.figure_display("fig_classic", fig_classic, (600, 0))
    imgui.same_line()
    immvision.figure_display("mandelbrot", mandelbrot_ax.figure, (600, 0))
    if imgui.button("Exit"):
        immvision.imgui_runner.exit_app()


immvision.run(gui2)
immvision.run(gui1)
immvision.run(gui2)
immvision.run(gui1)
# immvision.run(gui)
