#!/bin/bash
# build_opencv.sh — Build a minimalist static OpenCV for immvision.
#
# Single script used by both:
#   - CI (cibuildwheel before-all on Linux and Windows)
#   - CMake (find_opencv.cmake calls this at configure time on Linux/macOS)
#
# Usage: build_opencv.sh INSTALL_DIR [--full] [EXTRA_CMAKE_ARGS...]
#
#   --full    Build all OpenCV modules (not just the minimalist set)
#
# Environment variables:
#   IMMVISION_OPENCV_GIT_REPO   Use alternative git repo (e.g., local mirror)
#
# See docs/book/devel_docs/build_opencv_immvision.md for the full picture.
set -ex

INSTALL_DIR="$1"
shift

if [ -z "$INSTALL_DIR" ]; then
    echo "Usage: build_opencv.sh INSTALL_DIR [--full] [EXTRA_CMAKE_ARGS...]"
    exit 1
fi

FULL_BUILD=false
if [ "${1:-}" = "--full" ]; then
    FULL_BUILD=true
    shift
fi
EXTRA_CMAKE_ARGS=("$@")

OPENCV_VERSION="4.13.0"
WORK_DIR=$(mktemp -d)
SRC_DIR="$WORK_DIR/opencv-$OPENCV_VERSION"
BUILD_DIR="$WORK_DIR/opencv_build"

# Download OpenCV source
if [ -n "${IMMVISION_OPENCV_GIT_REPO}" ]; then
    git clone --depth 1 --branch "${OPENCV_VERSION}" "${IMMVISION_OPENCV_GIT_REPO}" "$SRC_DIR"
else
    curl -sL "https://github.com/opencv/opencv/archive/refs/tags/${OPENCV_VERSION}.tar.gz" \
        -o "$WORK_DIR/opencv.tar.gz"
    tar xzf "$WORK_DIR/opencv.tar.gz" -C "$WORK_DIR"
fi

mkdir -p "$BUILD_DIR"

# Parallel jobs
NPROC=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Build flags
if [ "$FULL_BUILD" = true ]; then
    OPENCV_CMAKE_ARGS=(
        -DBUILD_opencv_apps=OFF
        -DBUILD_TESTS=OFF
        -DBUILD_PERF_TESTS=OFF
    )
else
    # Minimalist static OpenCV: only core, imgcodecs, imgproc.
    OPENCV_CMAKE_ARGS=(
        -DBUILD_SHARED_LIBS=OFF
        -DBUILD_WITH_STATIC_CRT=OFF  # Use /MD (dynamic CRT) — required for Python extensions on Windows
        -DBUILD_opencv_apps=OFF
        -DBUILD_TESTS=OFF
        -DBUILD_PERF_TESTS=OFF

        -DWITH_OPENJPEG=OFF
        -DWITH_JASPER=OFF
        -DWITH_1394=OFF
        -DWITH_AVFOUNDATION=OFF
        -DWITH_CAP_IOS=OFF
        -DWITH_VTK=OFF
        -DWITH_CUDA=OFF
        -DWITH_CUFFT=OFF
        -DWITH_CUBLAS=OFF
        -DWITH_EIGEN=OFF
        -DWITH_FFMPEG=OFF
        -DWITH_GSTREAMER=OFF
        -DWITH_GTK=OFF
        -DWITH_GTK_2_X=OFF
        -DWITH_HALIDE=OFF
        -DWITH_VULKAN=OFF
        -DWITH_OPENEXR=OFF
        -DWITH_ADE=OFF
        -DWITH_AVIF=OFF
        -DWITH_AOM=OFF
        -DWITH_VMAF=OFF
        -DWITH_TIFF=OFF
        -DWITH_WEBP=OFF
        -DWITH_PROTOBUF=OFF

        -DWITH_JPEG=ON
        -DBUILD_JPEG=ON
        -DWITH_PNG=ON
        -DBUILD_PNG=ON
        -DWITH_SIMD=OFF  # Disabled on all platforms: avoids cross-compilation SIMD mismatch
                         # on Windows (ARM64 host → x64 target). Perf impact is negligible
                         # for this minimalist OpenCV (image loading only).

        -DBUILD_opencv_python2=OFF
        -DBUILD_opencv_python3=OFF
        -DBUILD_opencv_features2d=OFF
        -DBUILD_opencv_calib3d=OFF
        -DBUILD_opencv_dnn=OFF
        -DBUILD_opencv_flann=OFF
        -DBUILD_opencv_gapi=OFF
        -DBUILD_opencv_highgui=OFF
        -DBUILD_opencv_java=OFF
        -DBUILD_opencv_js=OFF
        -DBUILD_opencv_ml=OFF
        -DBUILD_opencv_objc=OFF
        -DBUILD_opencv_objdetect=OFF
        -DBUILD_opencv_photo=OFF
        -DBUILD_opencv_python=OFF
        -DBUILD_opencv_stiching=OFF
        -DBUILD_opencv_video=OFF
        -DBUILD_opencv_videoio=OFF
    )
fi

# Configure — no hardcoded generator or architecture; cmake auto-detects the compiler,
# and the caller passes architecture flags (e.g. -A x64) via EXTRA_CMAKE_ARGS.
cmake -S "$SRC_DIR" -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
    -DCMAKE_INSTALL_LIBDIR=lib \
    "${OPENCV_CMAKE_ARGS[@]}" \
    "${EXTRA_CMAKE_ARGS[@]}"

# Build & install
cmake --build "$BUILD_DIR" --config Release -j "$NPROC"
cmake --install "$BUILD_DIR" --config Release

# Create dummy 3rdparty files (OpenCVConfig.cmake may reference modules we didn't build)
for dir in lib/opencv4/3rdparty lib64/opencv4/3rdparty; do
    mkdir -p "$INSTALL_DIR/$dir"
    for f in liblibprotobuf.a libquirc.a libade.a libprotobuf.lib quirc.lib ade.lib; do
        echo "dummy" > "$INSTALL_DIR/$dir/$f"
    done
done

# Cleanup
rm -rf "$WORK_DIR"

echo "OpenCV $OPENCV_VERSION installed to $INSTALL_DIR"
