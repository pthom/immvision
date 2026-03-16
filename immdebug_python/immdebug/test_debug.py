import numpy as np
from immdebug import immdebug, immdebug_bgr

# Synthetic RGB test image: 3 horizontal bands (red, green, blue)
# Each band has a left-to-right gradient from black to full intensity
h, w = 300, 400
image = np.zeros((h, w, 3), dtype=np.uint8)
band_h = h // 3
gradient = np.linspace(0, 255, w, dtype=np.uint8)

image[:band_h, :, 0] = gradient           # Red band
image[band_h:2*band_h, :, 1] = gradient   # Green band
image[2*band_h:, :, 2] = gradient          # Blue band

# With immdebug (RGB): bands should appear as red, green, blue (correct)
immdebug(image, "RGB gradient (immdebug)")

# With immdebug_bgr: bands should appear as blue, green, red (wrong — proves BGR swap works)
immdebug_bgr(image, "RGB gradient (immdebug_bgr — expect swapped R/B)")
