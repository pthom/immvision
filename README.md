![Ubuntu](https://github.com/pthom/immvision/workflows/CppLib/badge.svg)

# ImmVision

ImmVision (a.k.a Immediate Vision) provides two tools for working with images:

- **ImmDebug** — a standalone image debugger for any C++ or Python process
- **ImmVision viewer** — an image viewer/analyzer widget for [ImGui](https://github.com/ocornut/imgui) applications


## ImmDebug — Visual Image Debugger

[Video tutorial on Youtube](https://www.youtube.com/watch?v=ztVBk2FN6_8)

ImmDebug lets you visually inspect images from any running program — during execution or even after it finishes (post-mortem). Just add one-line calls to send images to a standalone viewer with zoom, pan, pixel inspection, and colormaps.

![debugger](doc/immdebug.jpg)

### Python

Install from PyPI (includes both the client and the viewer):

```bash
pip install immdebug
```

**1. Start the viewer** (in a terminal):

```bash
immdebug-viewer
```

**2. Send images** (from your code, a script, a notebook, ...):

```python
import numpy as np
from immdebug import immdebug

image = np.random.randint(0, 255, (480, 640, 3), dtype=np.uint8)
immdebug(image, "random noise")
```

**Example with OpenCV:**

```python
import cv2
from immdebug import immdebug

image = cv2.imread("photo.jpg")
immdebug(image, "original")

gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
immdebug(gray, "grayscale")

edges = cv2.Canny(gray, 100, 200)
immdebug(edges, "edges")
```

See the [immdebug Python package](immdebug_python/) for the full API documentation.

### C++

#### Step 1: compile immdebug_viewer

```bash
git clone https://github.com/pthom/immvision.git
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j
```

Optionally, copy `bin/immdebug_viewer` somewhere in your PATH.

Then, launch the viewer, so that it receives and displays images sent from your code.


#### Step 2: add immdebug to your project

Simply drop the content of [src/immdebug](src/immdebug) into your project. Only OpenCV is required — no need to link to immvision.

```cpp
#include "immdebug/immdebug.h"

void ProcessImage()
{
    cv::Mat image = cv::imread("photo.jpg");
    ImmVision::ImmDebug(image, "original");

    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    ImmVision::ImmDebug(gray, "grayscale");

    cv::Mat edges;
    cv::Canny(gray, edges, 100, 200);
    ImmVision::ImmDebug(edges, "edges");
}
```

<details>
<summary>C++ API</summary>

```cpp
namespace ImmVision
{
    void ImmDebug(
        const cv::Mat & image,
        const std::string & legend = "",
        const cv::Point2d & zoomCenter = cv::Point2d(),
        double zoomRatio = -1.,
        const std::string& zoomKey = "",
        const std::string& colorAdjustmentsKey = "",
        bool isColorOrderBGR = true
    );
}
```
</details>

### ImmDebug features

- **Non-blocking**: just writes a file and returns immediately
- **Cross-language**: C++ and Python clients use the same binary format and work with either viewer
- **Single instance**: re-launching the viewer brings the existing instance to the top
- **Post-mortem**: images persist in `<temp_directory>/ImmDebug/` for 1 hour — start the viewer after your script finishes
- **Auto cleanup**: old images are cleaned automatically


## ImmVision — Image Viewer Widget for ImGui

[Video tutorial on YouTube](https://youtu.be/nuJW21-BCkE)

ImmVision provides an image viewer widget for applications built with [ImGui](https://github.com/ocornut/imgui). It supports zoom, pan, pixel value inspection, colormaps, linked views, and more.

This widget is included in [Dear ImGui Bundle](https://github.com/pthom/imgui_bundle), where it can be used from both C++ and Python.

See [online demo!](https://traineq.org/ImGuiBundle/emscripten/bin/demo_immvision_launcher.html)

![online_demo](doc/online_demo.jpg)

### C++ API

```cpp
IMMVISION_API void Image(const std::string& label, const cv::Mat& mat, ImageParams* params);
IMMVISION_API void ImageDisplay(const std::string& label, const cv::Mat& mat, const ImageParams& params);

IMMVISION_API void Inspector_AddImage(const cv::Mat& image, const std::string& legend, ...);
IMMVISION_API void Inspector_Show();
```

See [Full API](src/immvision/image.h)

### Python (via Dear ImGui Bundle)

```python
from imgui_bundle import immvision

immvision.image("My Image", my_mat, params)
immvision.inspector_add_image(my_mat, "label")
immvision.inspector_show()
```

See [Dear ImGui Bundle documentation](https://pthom.github.io/imgui_bundle/) for setup and usage.
