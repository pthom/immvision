![Ubuntu](https://github.com/pthom/immvision/workflows/CppLib/badge.svg)

# ImmVision

ImmVision (a.k.a Immediate Vision) is an image debugger and viewer.

## ImmDebug: standalone Image Debugger (during execution, *and* post-mortem)

[Video tutorial on Youtube](https://www.youtube.com/watch?v=ztVBk2FN6_8)


immvision includes an advanced image debugger which you can easily plug into your C++ or Python projects in order to be able to visually debug images inside your image processing algorithms, during execution or even *after* execution (post-mortem).

- **C++ client**: requires only OpenCV, no need to link to immvision (just copy 4 cpp files into your project)
- **Python client**: requires only numpy (no OpenCV, no compilation needed)
- **Python viewer**: a pure Python viewer is also available, using [imgui_bundle](https://github.com/pthom/imgui_bundle)

Let's consider an example where an image is modified in several steps, and each call to `ImmDebug` sends the intermediate result to the debugger.

**C++ example:**

```cpp
#include "immdebug/immdebug.h"

void ExampleImageProcessingWithDebug()
{
    cv::Mat image = cv::imread(ResourcesDir() + "/house.jpg");
    ImmVision::ImmDebug(image, "original");

    cv::Mat roi = image(cv::Rect(800, 600, 400, 400));
    ImmVision::ImmDebug(roi, "roi");

    cv::Mat black;
    cv::cvtColor(roi, black, cv::COLOR_RGB2GRAY);
    ImmVision::ImmDebug(black, "black");

    cv::Mat blur;
    cv::blur(black, blur, cv::Size(5, 5));
    ImmVision::ImmDebug(blur, "blur");

    cv::Mat floatImage;
    blur.convertTo(floatImage, CV_64FC1);
    floatImage = floatImage / 255.;
    ImmVision::ImmDebug(floatImage, "floatImage");

    cv::Mat sobel;
    cv::Sobel(floatImage, sobel, CV_64F, 1, 1);
    ImmVision::ImmDebug(sobel, "sobel");
}
```

**Python example:**

```python
import cv2
import numpy as np
from immdebug import immdebug

image = cv2.imread("house.jpg")
immdebug(image, "original")

roi = image[600:1000, 800:1200]
immdebug(roi, "roi")

gray = cv2.cvtColor(roi, cv2.COLOR_BGR2GRAY)
immdebug(gray, "gray")

blurred = cv2.blur(gray, (5, 5))
immdebug(blurred, "blur")

float_image = blurred.astype(np.float64) / 255.0
immdebug(float_image, "floatImage")

sobel = cv2.Sobel(float_image, cv2.CV_64F, 1, 1)
immdebug(sobel, "sobel")
```


immdebug is an external application, which will receive the images in real time and enable to analyse them at each step. It will then display:

![debugger](doc/immdebug.jpg)

### How to install the debugger into your program:

### How to use immdebug from Python

Install the `immdebug` package from PyPI:

```bash
pip install immdebug
```

**Start the viewer:**

```bash
immdebug-viewer
# or: python -m immdebug
```

**Send images from your code:**

```python
from immdebug import immdebug
immdebug(my_image, "step 1")
```

See the [immdebug Python package](immdebug_python/) for full documentation.

### How to use immdebug from C++

#### Step 1: compile immdebug_viewer

```bash
git clone https://github.com/pthom/immvision.git
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j
```

Optionally, copy bin/immdebug_viewer somewhere in your PATH.

#### Step 2: add immdebug to your project

Simply drop the content of [src/immdebug](src/immdebug) into your project.

The API is extremely simple:

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

#### Step 3: Add some calls to `ImmDebug` in your program

`ImmDebug` is non-blocking on the client side (all it does is to save a file in the temporary directory).


### Features

- **Non-blocking**: `immdebug` just writes a file and returns immediately
- **Cross-language**: C++ and Python clients use the same binary format, both work with either viewer
- **Single instance**: re-launching `immdebug_viewer` brings the existing instance to the top
- **Post-mortem**: images are written to `<temp_directory>/ImmDebug/` — launch the viewer later, they're still there
- **Auto cleanup**: images are cleaned automatically after 1 hour


## Image Viewer

[Video tutorial on YouTube](https://youtu.be/nuJW21-BCkE)


This image viewer runs inside [ImGui](https://github.com/ocornut/imgui) (and thus your program must be linked to it).

This viewer is also included inside [ImGui Bundle](https://github.com/pthom/imgui_bundle), where it can also be used from python.

The API can be summarized as 
```
IMMVISION_API void Image(const std::string& label, const cv::Mat& mat, ImageParams* params);
```

See [Full API](src/immvision/image.h)



See [online demo!](https://traineq.org/ImGuiBundle/emscripten/bin/demo_immvision_launcher.html)

![online_demo](doc/online_demo.jpg)

