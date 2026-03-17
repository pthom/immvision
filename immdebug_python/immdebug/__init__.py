"""immdebug: send images to the immdebug viewer for visual debugging.

Usage:
    import numpy as np
    from immdebug import immdebug

    image = np.random.randint(0, 255, (480, 640, 3), dtype=np.uint8)
    immdebug(image, "my image")

The viewer can be launched with:
    python -m immdebug.viewer
"""

from immdebug.client import immdebug, immdebug_bgr

__all__ = ["immdebug", "immdebug_bgr"]
