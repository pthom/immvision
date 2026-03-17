# ImmVision Changelog

## v1.92.601 (2026-03-17) — OpenCV is now optional / GPU rendering + DrawList annotations

**Breaking changes:**
- **OpenCV is no longer required.** ImmVision now works fully without OpenCV. When OpenCV is available, `cv::Mat` can still be passed to any ImmVision function via implicit zero-copy conversion (behind `IMMVISION_HAS_OPENCV`).
- **ImmDebug API changed:** `ImmDebug(image, ..., isColorOrderBGR)` replaced by `ImmDebug()` (assumes RGB) and `ImmDebugBgr()` (for BGR/OpenCV images). Python: `immdebug()` + `immdebug_bgr()`.
- **ImmDebug Python binary format changed** (incompatible with previous `immdebug` PyPI package). Update both client and viewer together.
- **`ImageBuffer` replaces `cv::Mat`** in all public C++ APIs. Lightweight container with `void* data`, width, height, channels, depth, stride. Supports zero-copy wrapping from any image source.

**New features:**
- `ImageBuffer` constructor from raw pointer: `ImageBuffer(data, w, h, ch)` — works with stb_image, SDL, custom buffers, etc.
- `ImRead()` — built-in image reader using stb_image (no OpenCV needed)
- Colormaps now work on all single-channel images (was float-only)
- Multithreaded pixel processing (warp/resize, colormap, RGBA conversion, alpha blending) using `hardware_concurrency()-1` threads. Sequential fallback on Emscripten.
- Custom geometric types (`Point`, `Point2d`, `Size`, `Matrix33d`) with optional bidirectional `cv::` conversions
- Python bindings use a new `ImageBuffer ↔ numpy` type caster (replaces cvnp_nano)
- iOS and Android support (fixed GL include paths)

**Removed:**
- cvnp and cvnp_nano submodules (replaced by built-in type caster)
- OpenCV prebuild step from wheel CI (wheels are ~16% smaller, ~6 min faster to build)

**Internal:**
- All ~450 internal `cv::` references replaced with ImmVision types
- `cv::imwrite` replaced by stb_image_write (PNG/JPG/BMP/TGA/HDR)
- Drawing primitives (lines, ellipses, text) reimplemented with bitmap font + Bresenham algorithms
- Zoom/pan warp reimplemented with custom nearest/bilinear/area interpolation
- Color conversion, image statistics, alpha blending reimplemented without OpenCV
- 27 C++ unit tests + 54 Python unit tests, both passing with and without OpenCV

**Rendering pipeline rewritten:**
- **GPU-based zoom/pan**: Image is uploaded to GPU texture once (with mipmaps), pan/zoom handled via UV coordinates. No more per-frame CPU warp.
- **Texture filtering**: `GL_NEAREST` (zoom ≥ 12x), `GL_LINEAR` (1x–12x), `GL_LINEAR_MIPMAP_LINEAR` (< 1x, for high-quality downsampling)
- **DrawList annotations**: Grid lines, pixel values, and watched pixel markers drawn via ImGui DrawList in screen space (instead of onto the image buffer)
- **DrawList backgrounds**: School paper and alpha checkerboard patterns drawn via DrawList (no longer composited into texture)
- **DrawList icons**: Zoom/adjust buttons drawn via DrawList (no more icon texture generation)
- **~900 lines removed**: Bresenham drawing, bitmap font, CPU interpolation code, icon texture cache

**Inspector redesign:**
- Horizontal filmstrip with scrolling replaces vertical listbox
- Adjustable thumbnail size via vertical slider
- Delete button appears on hover (DrawList circle + x)


## v1.0.0 – v1.92.600

ImmVision was originally built on top of OpenCV. It provided an immediate-mode image viewer widget for Dear ImGui with zoom, pan, pixel inspection, colormaps, and linked views. Available as part of [Dear ImGui Bundle](https://github.com/pthom/imgui_bundle) for both C++ and Python.
