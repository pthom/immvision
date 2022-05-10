# Study Mandelbrot

### Piste modernGL / Python
https://github.com/moderngl/moderngl/blob/master/examples/mandelbrot_set.py
=> très bien, très simple

### piste opencl (c++)
https://github.com/simonschoelly/OpenCl-Mandelbrot  // python!
Fonctionne si on edite requirements pour supprimer no de versions!
https://github.com/kurtamohler/mandelbrot-opencl // C++, chiant a compiler

### piste vulkan C++
https://github.com/Erkaman/vulkan_minimal_compute

### Piste gmp + glsl C++ (linux)
https://github.com/Grey41/Mandelbrot-Zoom
Brouillon, lent, et marche mal

### GLSL P

# Study Implot

## Pybinds implot:

### (py)mahi-gui : tres bonne source d'inspiration (pourrait remplace pyimgui)
See https://github.com/JoelLinn/py-mahi-gui   (6 stars)
Licence a la MIT
Fournit un fichier de bindings pour implot
pybind de https://github.com/mahilab/mahi-gui (Par E Pezent):   (349 stars) (Dirt Simple C++ GUI Toolkit using GLFW, ImGui, and NanoVG)

pip install mahi-gui

### https://github.com/0lru/p3ui (C++ & python UI lib) 22 stars
ImGui + Implot + skia (2D graphics library for chrome, android, etc: https://skia.org/)

Trop gros, trop de dépendances, complexe a builder.

### Other: see https://github.com/hinxx/pyimplot (tentative cython a la pyimgui Avril 2021)

## Implot Heatmap: see discussion at epezent/implot (search python or gallery)
https://github.com/epezent/implot/discussions/180#discussioncomment-1016572
cf backend branch / GPU accelerated heatmaps
==>  un viewer de heatmap / pas de concurrence avec immvision
Voir https://github.com/epezent/implot_demos
