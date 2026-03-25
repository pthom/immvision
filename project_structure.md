```
immvision/src/                              Source code for immvision
├── CMakeLists.txt                          Main CMake file
├── immvision/                              Main immvision library (and include path)
│     ├── CMakeLists.txt
│     │
│     │   ── Public API ──────────────────────────────────────────────────
│     │
│     ├── immvision.h                       Convenience header: includes immvision_types.h, image.h, inspector.h, gl_texture.h
│     ├── immvision_types.h                 Common image types (ImageBuffer, Point, Rect, Size, Matrix33d, Color4d, ImageDepth)
│     ├── immvision_types.cpp               ImageBuffer implementation (creation, clone, zero-copy cv::Mat interop, Matrix33d math)
│     ├── image.h                           Main API: Image() display widget, ImageDisplay(), color order, ImageParams, ColormapSettingsData
│     ├── inspector.h                       Multi-image inspector: Inspector_AddImage(), Inspector_Show(), Inspector_ClearImages()
│     ├── gl_texture.h                      GlTexture: GPU texture wrapper (create/update from ImageBuffer, RAII, non-copyable, movable)
│     ├── imgui_imm.h                       ImGui extensions: SliderAnyFloat, ComputeDisplayImageSize, GroupPanel, SameLineAlignRight
│     │
│     │   ── Internal implementation ─────────────────────────────────────
│     │
│     └── internal/
│         │
│         │   Top-level internal files (main widget logic, caching, serialization)
│         │
│         ├── image.cpp                     Main Image() widget implementation (organized as lambdas for zoom, colormap, overlays, etc.)
│         │                                 Also: color order stack (UseRgbColorOrder/UseBgrColorOrder), ImageFromFile/ImageToFile
│         ├── image_cache.h                 ImageTextureCache: caches params + GL textures, linked zoom/colormap propagation
│         ├── image_cache.cpp               Cache update logic, RGBA refresh decisions, zoom-to-display-size adaptation
│         ├── image_params_serialize.cpp    JSON serialization of ImageParams (conditional on IMMVISION_SERIALIZE_JSON)
│         ├── inspector.cpp                 Inspector UI: static image list, tabbed display, per-image ImageParams
│         │
│         │   ── cv/ ── Image processing (no ImGui dependency) ───────────
│         │
│         ├── cv/
│         │     ├── colormap.h              Colormap operations: apply, settings, available colormaps list, GUI
│         │     ├── colormap.cpp            Colormap application (via tinycolormap), histogram stats, interactive colormap GUI
│         │     ├── convert_to_rgba.h       Convert any ImageBuffer to RGBA uint8 (handles all depths + BGR swap)
│         │     ├── convert_to_rgba.cpp     Template-based conversion with parallel_for optimization
│         │     ├── matrix_info_utils.h     Pixel inspection: type name, pixel values at (x,y), color info string
│         │     ├── matrix_info_utils.cpp   Pixel value extraction for all depths and channel counts
│         │     ├── zoom_pan_transform.h    2D zoom/pan math: matrices, UV computation, visible ROI
│         │     └── zoom_pan_transform.cpp  Matrix operations for zoom/pan/scale, UV coordinate computation for GPU rendering
│         │
│         │   ── drawing/ ── Overlay rendering via ImDrawList ────────────
│         │
│         ├── drawing/
│         │     ├── draw_list_annotate.h    Grid, pixel values, watched pixels overlays; checkerboard & school paper backgrounds
│         │     ├── draw_list_annotate.cpp  Coordinate-aware overlay drawing in screen space
│         │     ├── internal_icons.h        Vector icon buttons (zoom +/-, scale 1:1, full view, adjust levels)
│         │     └── internal_icons.cpp      DrawList-based icon rendering (no texture atlas needed)
│         │
│         │   ── gl/ ── OpenGL texture management ────────────────────────
│         │
│         ├── gl/
│         │     ├── gl_provider.h           Low-level GL operations: GenTexture, DeleteTexture, Blit_RGBA_Buffer, SetTextureFiltering
│         │     ├── gl_provider.cpp         OpenGL calls (texture upload, mipmap generation, filtering modes)
│         │     ├── gl_texture.cpp          GlTexture implementation (create, update from ImageBuffer via convert_to_rgba + gl_provider)
│         │     ├── short_lived_cache.h     Generic TTL-based cache: Cache<K,V>, ShortLivedValue<V>, ShortLivedCache<K,V>
│         │     └── short_lived_cache.cpp   TimerSeconds() implementation
│         │
│         │   ── imgui/ ── ImGui integration widgets ─────────────────────
│         │
│         ├── imgui/
│         │     ├── image_widgets.h         Image display with mouse tracking (UV-aware), pixel color widget, image info
│         │     ├── image_widgets.cpp       ImGui::Image() wrappers with hover detection and mouse coordinate tracking
│         │     └── imgui_imm.cpp           ImGui utility extensions (SliderAnyFloat, GroupPanel, BeginGroupFixedWidth)
│         │
│         │   ── misc/ ── Standalone utilities (no ImmVision-specific deps) ──
│         │
│         ├── misc/
│         │     ├── panic.h                 Error handling: Panic() with debug break
│         │     ├── panic.cpp               Panic implementation (logs + debug_break on debug builds)
│         │     ├── debugbreak.h            Cross-platform debug break (external: Scott Tsai)
│         │     ├── math_utils.h            Min, max, lerp, unlerp, arange, round
│         │     ├── math_utils.cpp          MaximumValue, MinimumValue, RoundInt
│         │     ├── string_utils.h          String utilities: split, join, indent, ToString() overloads for all types
│         │     ├── string_utils.cpp        LowerString, JoinStrings, SplitString, IndentLine/IndentLines
│         │     ├── immvision_to_string.h   Autogenerated ToString() for ImageParams, ColormapSettingsData, MouseInformation
│         │     ├── immvision_to_string.cpp Autogenerated serialization to human-readable strings
│         │     ├── insertion_order_map.h    HashMap preserving insertion order (unordered_map + vector<Key>)
│         │     ├── insertion_order_map.cpp  (minimal, supports the header)
│         │     ├── parallel_for.h          Thread pool for parallel image processing (leaves one core spare)
│         │     ├── portable_file_dialogs.h Cross-platform file dialogs (external library)
│         │     ├── magic_enum.hpp          Enum reflection (external: Daniil Goncharov)
│         │     └── tinycolormap.hpp        Colormap LUTs (external: Yuki Koyama)
│         │
│         │   ── stb/ ── Image I/O (external libraries) ─────────────────
│         │
│         └── stb/
│             ├── stb_image.h               Image loading: PNG, JPG, BMP, etc. (external: Sean Barrett)
│             ├── stb_image_impl.cpp        stb_image implementation compilation unit
│             ├── stb_image_write.h         Image saving: PNG, JPG, BMP (external: Sean Barrett)
│             └── stb_image_write_impl.cpp  stb_image_write implementation compilation unit
│
├── immdebug/                               Remote image debugging library
│     ├── CMakeLists.txt
│     ├── immdebug.h                        Public API for remote image debugging
│     ├── immdebug.cpp
│     ├── immdebug_internal.h
│     └── immdebug_internal.cpp
│
├── immdebug_viewer/                        Standalone viewer app for immdebug
│     ├── CMakeLists.txt
│     ├── immdebug_viewer.cpp               Viewer application main
│     ├── single_instance_app.h             Ensures only one viewer instance runs
│     └── single_instance_app.cpp
│
├── immvision_gl_loader/                    OpenGL loader (glad-based)
│     ├── CMakeLists.txt
│     ├── immvision_gl_loader.h             GL loader initialization API
│     ├── immvision_gl_loader.cpp
│     └── glad/                             glad library (auto-generated OpenGL bindings)
│           ├── include/
│           │     ├── KHR/khrplatform.h
│           │     └── glad/glad.h
│           └── src/glad.c
│
├── demos_immvision/                        Demo applications
│     ├── CMakeLists.txt
│     ├── demo_immdebug/                    Demo for remote image debugging
│     │     ├── CMakeLists.txt
│     │     └── demo_immdebug.main.cpp
│     ├── demo_immvision/                   Demo for Image() widget
│     │     ├── CMakeLists.txt
│     │     └── demo_immvision.main.cpp
│     ├── demo_inspector/                   Demo for the multi-image inspector
│     │     ├── CMakeLists.txt
│     │     ├── datestr.h
│     │     └── demo_inspector.main.cpp
│     └── resources/                        Sample images for demos
│         ├── bear_transparent.png
│         ├── dmla.jpg
│         ├── house.jpg
│         └── tennis.jpg
│
├── tests/                                  Unit tests
│   ├── CMakeLists.txt
│   ├── test_immvision_internals.cpp

```


## Architecture overview

Dependencies flow in one direction:

```
  misc/  (standalone utilities, no ImmVision deps)
    ↓
  cv/    (image processing: colormap, zoom/pan, format conversion)
    ↓
  gl/    (GPU: texture management, TTL caching)
    ↓
  drawing/ + imgui/  (UI: overlays via DrawList, image widgets)
    ↓
  image.cpp + image_cache.cpp + inspector.cpp  (top-level widget logic)
```

**Public API** is minimal (7 files). All implementation details are in `internal/`.

**Key design patterns:**
- `ImageTextureCache` (image_cache.h) is the central caching system: it stores both lightweight
  params (persist forever) and heavy image+texture data (TTL-based, default 5s).
- Linked images: multiple Image() widgets can share zoom/colormap via `ZoomKey`/`ColormapKey`
  strings; changes propagate through `UpdateLinkedZooms()`/`UpdateLinkedColormapSettings()`.
- GPU pipeline: images are converted to RGBA, uploaded once as a GL texture with mipmaps;
  zoom/pan is handled via UV coordinates and DrawList overlays (no re-upload on pan/zoom).
- `GlTexture` is RAII, non-copyable, movable — prevents accidental GPU resource leaks.

**External (vendored) libraries in misc/ and stb/:**
- `tinycolormap.hpp` — colormap LUTs
- `magic_enum.hpp` — enum reflection
- `debugbreak.h` — cross-platform debug break
- `portable_file_dialogs.h` — native file dialogs
- `stb_image.h` / `stb_image_write.h` — image I/O