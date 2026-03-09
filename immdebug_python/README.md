# immdebug

Visual image debugger for Python — inspect images from any running process with zoom, pan, pixel values, and colormaps.

Part of the [immvision](https://github.com/pthom/immvision) / [Dear ImGui Bundle](https://github.com/pthom/imgui_bundle) ecosystem.

## Install

```bash
pip install immdebug
```

This installs both the client (for sending images) and the viewer (for displaying them).

## Quick start

**1. Start the viewer** (in a terminal):

```bash
immdebug-viewer
# or: python -m immdebug
```

**2. Send images** (from your code, a script, a notebook, ...):

```python
import numpy as np
from immdebug import immdebug

image = np.random.randint(0, 255, (480, 640, 3), dtype=np.uint8)
immdebug(image, "random noise")
```

The image appears in the viewer immediately.

## How it works

`immdebug()` serializes the image to a temp directory (`<tempdir>/ImmDebug/`). The viewer polls this directory and displays incoming images using [immvision](https://github.com/pthom/immvision)'s inspector.

This is the same protocol used by the C++ `ImmDebug()` function, so the Python client works with both the Python and C++ viewers.

## Use with OpenCV

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

Images default to BGR channel order (OpenCV convention). For RGB images (e.g. from PIL or matplotlib), pass `is_color_order_bgr=False`.

## API

```python
def immdebug(
    image: np.ndarray,
    legend: str = "",
    zoom_center: tuple[float, float] = (0.0, 0.0),
    zoom_ratio: float = -1.0,
    zoom_key: str = "",
    color_adjustments_key: str = "",
    is_color_order_bgr: bool = True,
) -> None:
```

| Parameter | Description |
|---|---|
| `image` | numpy array — HxW (grayscale) or HxWxC (color). Supports uint8, int8, uint16, int16, int32, float32, float64. |
| `legend` | Display name in the viewer |
| `zoom_center` | Initial zoom center (x, y) |
| `zoom_ratio` | Initial zoom ratio (-1 for auto-fit) |
| `zoom_key` | Link zoom/pan across images sharing this key |
| `color_adjustments_key` | Link color adjustments across images sharing this key |
| `is_color_order_bgr` | True for BGR (OpenCV), False for RGB (PIL, matplotlib) |

## Features

- **Non-blocking**: `immdebug()` just writes a file and returns immediately
- **Post-mortem**: images persist in the temp directory for 1 hour — start the viewer after your script finishes
- **Single instance**: launching a second viewer brings the existing one to the front
- **Cross-language**: works with both the Python and C++ immdebug viewers

## License

MIT
