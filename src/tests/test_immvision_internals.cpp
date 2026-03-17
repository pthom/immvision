// Unit tests for ImmVision internal types and functions.
// No GUI or OpenGL required — tests pure computation only.
//
// Build: add_executable(test_immvision_internals test_immvision_internals.cpp)
//        target_link_libraries(test_immvision_internals immvision)

#include "immvision/immvision_types.h"
#include "immvision/internal/cv/convert_to_rgba.h"
#include "immvision/internal/cv/zoom_pan_transform.h"
#include "immvision/internal/cv/colormap.h"
#include "immvision/internal/cv/matrix_info_utils.h"

#include <cassert>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <vector>

using namespace ImmVision;

// =========================================================================
// Helpers
// =========================================================================

static int sTestCount = 0;
static int sPassCount = 0;

#define TEST(name) \
    do { sTestCount++; printf("  TEST: %s ... ", name); } while(0)

#define PASS() \
    do { sPassCount++; printf("OK\n"); } while(0)

#define CHECK(cond) \
    do { if (!(cond)) { printf("FAIL at %s:%d: %s\n", __FILE__, __LINE__, #cond); exit(1); } } while(0)

#define CHECK_NEAR(a, b, eps) \
    do { if (std::fabs((a) - (b)) > (eps)) { printf("FAIL at %s:%d: %g != %g (eps=%g)\n", __FILE__, __LINE__, (double)(a), (double)(b), (double)(eps)); exit(1); } } while(0)


// =========================================================================
// ImageBuffer tests
// =========================================================================

void test_image_buffer_zeros()
{
    TEST("ImageBuffer::Zeros");
    ImageBuffer buf = ImageBuffer::Zeros(10, 5, 3, ImageDepth::uint8);
    CHECK(!buf.empty());
    CHECK(buf.width == 10);
    CHECK(buf.height == 5);
    CHECK(buf.channels == 3);
    CHECK(buf.depth == ImageDepth::uint8);
    CHECK(buf.step == 10 * 3 * 1);
    CHECK(buf.rows() == 5);
    CHECK(buf.cols() == 10);
    CHECK(buf.size().width == 10);
    CHECK(buf.size().height == 5);
    CHECK(buf.elemSize() == 1);
    CHECK(buf.elemSizeTotal() == 3);

    // All zeros
    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 30; x++)
            CHECK(buf.ptr<uint8_t>(y)[x] == 0);

    // Float variant
    ImageBuffer fbuf = ImageBuffer::Zeros(4, 3, 1, ImageDepth::float32);
    CHECK(fbuf.elemSize() == 4);
    CHECK(fbuf.step == 4 * 1 * 4);
    for (int y = 0; y < 3; y++)
        for (int x = 0; x < 4; x++)
            CHECK(fbuf.ptr<float>(y)[x] == 0.0f);

    PASS();
}

void test_image_buffer_clone()
{
    TEST("ImageBuffer::clone");
    ImageBuffer src = ImageBuffer::Zeros(4, 3, 1, ImageDepth::uint8);
    src.ptr<uint8_t>(0)[0] = 42;
    src.ptr<uint8_t>(1)[2] = 99;
    src.ptr<uint8_t>(2)[3] = 255;

    ImageBuffer dst = src.clone();
    CHECK(dst.width == 4 && dst.height == 3 && dst.channels == 1);
    CHECK(dst.ptr<uint8_t>(0)[0] == 42);
    CHECK(dst.ptr<uint8_t>(1)[2] == 99);
    CHECK(dst.ptr<uint8_t>(2)[3] == 255);

    // Verify deep copy — modifying dst doesn't affect src
    dst.ptr<uint8_t>(0)[0] = 0;
    CHECK(src.ptr<uint8_t>(0)[0] == 42);

    // Empty clone
    ImageBuffer empty;
    ImageBuffer emptyClone = empty.clone();
    CHECK(emptyClone.empty());

    PASS();
}

void test_image_buffer_ptr()
{
    TEST("ImageBuffer::ptr<T>");
    ImageBuffer buf = ImageBuffer::Zeros(3, 2, 2, ImageDepth::uint16);
    // Write via ptr
    buf.ptr<uint16_t>(0)[0] = 1000;  // pixel (0,0), channel 0
    buf.ptr<uint16_t>(0)[1] = 2000;  // pixel (0,0), channel 1
    buf.ptr<uint16_t>(0)[4] = 3000;  // pixel (2,0), channel 0
    buf.ptr<uint16_t>(1)[2] = 5000;  // pixel (1,1), channel 0

    // Read back
    CHECK(buf.ptr<uint16_t>(0)[0] == 1000);
    CHECK(buf.ptr<uint16_t>(0)[1] == 2000);
    CHECK(buf.ptr<uint16_t>(0)[4] == 3000);
    CHECK(buf.ptr<uint16_t>(1)[2] == 5000);

    // Const access
    const ImageBuffer& cbuf = buf;
    CHECK(cbuf.ptr<uint16_t>(0)[0] == 1000);

    PASS();
}

void test_image_buffer_sub_image()
{
    TEST("ImageBuffer::subImage");
    // Create a 4x4 uint8 1-channel image with known values
    ImageBuffer buf = ImageBuffer::Zeros(4, 4, 1, ImageDepth::uint8);
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            buf.ptr<uint8_t>(y)[x] = (uint8_t)(y * 10 + x);

    // Extract 2x2 sub-image at (1,1)
    ImageBuffer sub = buf.subImage(Rect(1, 1, 2, 2));
    CHECK(sub.width == 2);
    CHECK(sub.height == 2);
    CHECK(sub.channels == 1);
    CHECK(sub.step == buf.step);  // Same stride as parent

    // Verify values: sub(0,0) should be buf(1,1) = 11
    CHECK(sub.ptr<uint8_t>(0)[0] == 11);
    CHECK(sub.ptr<uint8_t>(0)[1] == 12);
    CHECK(sub.ptr<uint8_t>(1)[0] == 21);
    CHECK(sub.ptr<uint8_t>(1)[1] == 22);

    // Modify sub-image — should affect parent (shared memory)
    sub.ptr<uint8_t>(0)[0] = 99;
    CHECK(buf.ptr<uint8_t>(1)[1] == 99);

    // Multi-channel sub-image
    ImageBuffer rgb = ImageBuffer::Zeros(4, 4, 3, ImageDepth::uint8);
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
        {
            rgb.ptr<uint8_t>(y)[x * 3 + 0] = (uint8_t)(y * 10 + x);
            rgb.ptr<uint8_t>(y)[x * 3 + 1] = (uint8_t)(y * 10 + x + 100);
            rgb.ptr<uint8_t>(y)[x * 3 + 2] = (uint8_t)(y * 10 + x + 50);
        }
    ImageBuffer rgbSub = rgb.subImage(Rect(2, 1, 2, 2));
    CHECK(rgbSub.ptr<uint8_t>(0)[0] == 12);     // channel 0 of pixel (2,1)
    CHECK(rgbSub.ptr<uint8_t>(0)[1] == 112);    // channel 1 of pixel (2,1)

    PASS();
}

void test_image_buffer_copy_to()
{
    TEST("ImageBuffer::copyTo");
    ImageBuffer src = ImageBuffer::Zeros(3, 2, 1, ImageDepth::uint8);
    src.ptr<uint8_t>(0)[0] = 10;
    src.ptr<uint8_t>(0)[1] = 20;
    src.ptr<uint8_t>(0)[2] = 30;
    src.ptr<uint8_t>(1)[0] = 40;
    src.ptr<uint8_t>(1)[1] = 50;
    src.ptr<uint8_t>(1)[2] = 60;

    // Copy into a larger image's sub-region
    ImageBuffer dst = ImageBuffer::Zeros(5, 4, 1, ImageDepth::uint8);
    ImageBuffer dstSub = dst.subImage(Rect(1, 1, 3, 2));
    src.copyTo(dstSub);

    // Check the copied region
    CHECK(dst.ptr<uint8_t>(1)[1] == 10);
    CHECK(dst.ptr<uint8_t>(1)[2] == 20);
    CHECK(dst.ptr<uint8_t>(1)[3] == 30);
    CHECK(dst.ptr<uint8_t>(2)[1] == 40);
    CHECK(dst.ptr<uint8_t>(2)[2] == 50);
    CHECK(dst.ptr<uint8_t>(2)[3] == 60);

    // Check that surrounding pixels are still zero
    CHECK(dst.ptr<uint8_t>(0)[0] == 0);
    CHECK(dst.ptr<uint8_t>(0)[1] == 0);
    CHECK(dst.ptr<uint8_t>(1)[0] == 0);
    CHECK(dst.ptr<uint8_t>(1)[4] == 0);
    CHECK(dst.ptr<uint8_t>(3)[1] == 0);

    PASS();
}

void test_image_buffer_fill()
{
    TEST("ImageBuffer::fill");
    // uint8 RGBA
    ImageBuffer rgba = ImageBuffer::Zeros(3, 2, 4, ImageDepth::uint8);
    rgba.fill(Color4d(10, 20, 30, 200));
    for (int y = 0; y < 2; y++)
        for (int x = 0; x < 3; x++)
        {
            const uint8_t* p = rgba.ptr<uint8_t>(y) + x * 4;
            CHECK(p[0] == 10);
            CHECK(p[1] == 20);
            CHECK(p[2] == 30);
            CHECK(p[3] == 200);
        }

    // float32 1-channel
    ImageBuffer f32 = ImageBuffer::Zeros(2, 2, 1, ImageDepth::float32);
    f32.fill(Color4d(0.5, 0, 0, 0));
    for (int y = 0; y < 2; y++)
        for (int x = 0; x < 2; x++)
            CHECK_NEAR(f32.ptr<float>(y)[x], 0.5f, 1e-6);

    PASS();
}


// =========================================================================
// Type tests
// =========================================================================

void test_color4d()
{
    TEST("Color4d");
    Color4d c(10, 20, 30, 255);
    CHECK(c[0] == 10);
    CHECK(c[1] == 20);
    CHECK(c[2] == 30);
    CHECK(c[3] == 255);

    Color4d d;
    CHECK(d[0] == 0);
    CHECK(d[3] == 255);  // default alpha

    PASS();
}

void test_rect()
{
    TEST("Rect");
    Rect r(10, 20, 30, 40);
    CHECK(r.x == 10 && r.y == 20 && r.width == 30 && r.height == 40);
    CHECK(!r.empty());
    CHECK(r.area() == 1200);
    CHECK(r.contains(Point(10, 20)));
    CHECK(r.contains(Point(39, 59)));
    CHECK(!r.contains(Point(40, 20)));  // x >= x + width
    CHECK(!r.contains(Point(10, 60)));  // y >= y + height
    CHECK(!r.contains(Point(9, 20)));

    // Two-corners constructor
    Rect r2(Point(5, 10), Point(15, 30));
    CHECK(r2.x == 5 && r2.y == 10 && r2.width == 10 && r2.height == 20);

    // Empty rect
    Rect empty;
    CHECK(empty.empty());

    PASS();
}

void test_point2d_arithmetic()
{
    TEST("Point2d arithmetic");
    Point2d a(3.0, 4.0);
    Point2d b(1.0, 2.0);
    Point2d sum = a + b;
    Point2d diff = a - b;
    CHECK_NEAR(sum.x, 4.0, 1e-10);
    CHECK_NEAR(sum.y, 6.0, 1e-10);
    CHECK_NEAR(diff.x, 2.0, 1e-10);
    CHECK_NEAR(diff.y, 2.0, 1e-10);

    // Conversion from Point
    Point p(5, 7);
    Point2d pd(p);
    CHECK_NEAR(pd.x, 5.0, 1e-10);
    CHECK_NEAR(pd.y, 7.0, 1e-10);

    PASS();
}

void test_matrix33d_apply()
{
    TEST("Matrix33d::apply");
    // Identity
    Matrix33d eye = Matrix33d::eye();
    Point2d p(3.0, 4.0);
    Point2d r = eye.apply(p);
    CHECK_NEAR(r.x, 3.0, 1e-10);
    CHECK_NEAR(r.y, 4.0, 1e-10);

    // Translation
    Matrix33d t = Matrix33d::eye();
    t(0, 2) = 10.0;
    t(1, 2) = 20.0;
    r = t.apply(p);
    CHECK_NEAR(r.x, 13.0, 1e-10);
    CHECK_NEAR(r.y, 24.0, 1e-10);

    // Scale 2x
    Matrix33d s = Matrix33d::eye();
    s(0, 0) = 2.0;
    s(1, 1) = 2.0;
    r = s.apply(p);
    CHECK_NEAR(r.x, 6.0, 1e-10);
    CHECK_NEAR(r.y, 8.0, 1e-10);

    PASS();
}

void test_image_depth_helpers()
{
    TEST("ImageDepthName / ImageDepthIsFloat");
    CHECK(ImageDepthName(ImageDepth::uint8) == "uint8");
    CHECK(ImageDepthName(ImageDepth::float32) == "float32");
    CHECK(ImageDepthName(ImageDepth::float64) == "float64");
    CHECK(ImageDepthName(ImageDepth::int16) == "int16");

    CHECK(!ImageDepthIsFloat(ImageDepth::uint8));
    CHECK(!ImageDepthIsFloat(ImageDepth::int32));
    CHECK(ImageDepthIsFloat(ImageDepth::float32));
    CHECK(ImageDepthIsFloat(ImageDepth::float64));

    PASS();
}


// =========================================================================
// converted_to_rgba_image tests
// =========================================================================

void test_converted_to_rgba_uint8_3ch()
{
    TEST("converted_to_rgba: uint8 3ch RGB");
    ImageBuffer img = ImageBuffer::Zeros(2, 1, 3, ImageDepth::uint8);
    uint8_t* row = img.ptr<uint8_t>(0);
    row[0] = 10; row[1] = 20; row[2] = 30;   // pixel 0: R=10, G=20, B=30
    row[3] = 40; row[4] = 50; row[5] = 60;   // pixel 1

    ImageBuffer rgba = converted_to_rgba_image(img, false /* RGB order */);
    CHECK(rgba.width == 2 && rgba.height == 1 && rgba.channels == 4);
    CHECK(rgba.depth == ImageDepth::uint8);

    const uint8_t* out = rgba.ptr<uint8_t>(0);
    CHECK(out[0] == 10 && out[1] == 20 && out[2] == 30 && out[3] == 255);
    CHECK(out[4] == 40 && out[5] == 50 && out[6] == 60 && out[7] == 255);

    PASS();
}

void test_converted_to_rgba_uint8_3ch_bgr()
{
    TEST("converted_to_rgba: uint8 3ch BGR");
    ImageBuffer img = ImageBuffer::Zeros(1, 1, 3, ImageDepth::uint8);
    uint8_t* row = img.ptr<uint8_t>(0);
    row[0] = 100; row[1] = 150; row[2] = 200;  // B=100, G=150, R=200

    ImageBuffer rgba = converted_to_rgba_image(img, true /* BGR order */);
    const uint8_t* out = rgba.ptr<uint8_t>(0);
    // Should swap R and B: output is RGBA so R=200, G=150, B=100
    CHECK(out[0] == 200 && out[1] == 150 && out[2] == 100 && out[3] == 255);

    PASS();
}

void test_converted_to_rgba_uint8_1ch()
{
    TEST("converted_to_rgba: uint8 1ch grayscale");
    ImageBuffer img = ImageBuffer::Zeros(2, 1, 1, ImageDepth::uint8);
    img.ptr<uint8_t>(0)[0] = 128;
    img.ptr<uint8_t>(0)[1] = 255;

    ImageBuffer rgba = converted_to_rgba_image(img, false);
    const uint8_t* out = rgba.ptr<uint8_t>(0);
    CHECK(out[0] == 128 && out[1] == 128 && out[2] == 128 && out[3] == 255);
    CHECK(out[4] == 255 && out[5] == 255 && out[6] == 255 && out[7] == 255);

    PASS();
}

void test_converted_to_rgba_uint8_4ch()
{
    TEST("converted_to_rgba: uint8 4ch RGBA passthrough");
    ImageBuffer img = ImageBuffer::Zeros(1, 1, 4, ImageDepth::uint8);
    uint8_t* row = img.ptr<uint8_t>(0);
    row[0] = 10; row[1] = 20; row[2] = 30; row[3] = 128;

    ImageBuffer rgba = converted_to_rgba_image(img, false);
    const uint8_t* out = rgba.ptr<uint8_t>(0);
    CHECK(out[0] == 10 && out[1] == 20 && out[2] == 30 && out[3] == 128);

    PASS();
}

void test_converted_to_rgba_float32_1ch()
{
    TEST("converted_to_rgba: float32 1ch");
    ImageBuffer img = ImageBuffer::Zeros(3, 1, 1, ImageDepth::float32);
    img.ptr<float>(0)[0] = 0.0f;
    img.ptr<float>(0)[1] = 0.5f;
    img.ptr<float>(0)[2] = 1.0f;

    ImageBuffer rgba = converted_to_rgba_image(img, false);
    const uint8_t* out = rgba.ptr<uint8_t>(0);
    // 0.0 -> 0, 0.5 -> 127 or 128, 1.0 -> 255
    CHECK(out[0] == 0);
    CHECK(out[4] >= 127 && out[4] <= 128);
    CHECK(out[8] == 255);
    // All should be grayscale with alpha 255
    CHECK(out[0] == out[1] && out[1] == out[2] && out[3] == 255);

    PASS();
}

void test_converted_to_rgba_int16_1ch()
{
    TEST("converted_to_rgba: int16 1ch");
    ImageBuffer img = ImageBuffer::Zeros(2, 1, 1, ImageDepth::int16);
    img.ptr<int16_t>(0)[0] = -32768;  // min -> 0
    img.ptr<int16_t>(0)[1] = 32767;   // max -> 255

    ImageBuffer rgba = converted_to_rgba_image(img, false);
    const uint8_t* out = rgba.ptr<uint8_t>(0);
    CHECK(out[0] == 0);    // min maps to 0
    CHECK(out[4] == 255);  // max maps to 255

    PASS();
}


// WarpAffineScaleTranslate tests removed — CPU warp replaced by GPU texture sampling

// =========================================================================
// Colormap tests
// =========================================================================

void test_can_colormap()
{
    TEST("Colormap::CanColormap");
    ImageBuffer f32_1ch = ImageBuffer::Zeros(10, 10, 1, ImageDepth::float32);
    CHECK(Colormap::CanColormap(f32_1ch));

    ImageBuffer f64_1ch = ImageBuffer::Zeros(10, 10, 1, ImageDepth::float64);
    CHECK(Colormap::CanColormap(f64_1ch));

    ImageBuffer u8_1ch = ImageBuffer::Zeros(10, 10, 1, ImageDepth::uint8);
    CHECK(Colormap::CanColormap(u8_1ch));

    ImageBuffer i16_1ch = ImageBuffer::Zeros(10, 10, 1, ImageDepth::int16);
    CHECK(Colormap::CanColormap(i16_1ch));

    ImageBuffer f32_3ch = ImageBuffer::Zeros(10, 10, 3, ImageDepth::float32);
    CHECK(!Colormap::CanColormap(f32_3ch));

    PASS();
}

void test_apply_colormap()
{
    TEST("Colormap::ApplyColormap");
    // Create a 3-pixel float32 image: 0.0, 0.5, 1.0
    ImageBuffer img = ImageBuffer::Zeros(3, 1, 1, ImageDepth::float32);
    img.ptr<float>(0)[0] = 0.0f;
    img.ptr<float>(0)[1] = 0.5f;
    img.ptr<float>(0)[2] = 1.0f;

    ColormapSettingsData settings;
    settings.Colormap = "Viridis";
    settings.ColormapScaleMin = 0.0;
    settings.ColormapScaleMax = 1.0;

    ImageBuffer result = Colormap::ApplyColormap(img, settings);
    CHECK(result.width == 3 && result.height == 1);
    CHECK(result.channels == 4);
    CHECK(result.depth == ImageDepth::uint8);

    // The three pixels should have different colors (Viridis goes from dark purple to yellow)
    const uint8_t* p0 = result.ptr<uint8_t>(0);
    const uint8_t* p1 = result.ptr<uint8_t>(0) + 4;
    const uint8_t* p2 = result.ptr<uint8_t>(0) + 8;

    // All should have alpha = 255
    CHECK(p0[3] == 255);
    CHECK(p1[3] == 255);
    CHECK(p2[3] == 255);

    // Pixels should be different from each other
    bool p0_diff_p1 = (p0[0] != p1[0]) || (p0[1] != p1[1]) || (p0[2] != p1[2]);
    bool p1_diff_p2 = (p1[0] != p2[0]) || (p1[1] != p2[1]) || (p1[2] != p2[2]);
    CHECK(p0_diff_p1);
    CHECK(p1_diff_p2);

    PASS();
}


// =========================================================================
// MatrixInfoUtils tests
// =========================================================================

void test_mat_pixel_color_info()
{
    TEST("MatrixInfoUtils::MatPixelColorInfo");
    ImageBuffer img = ImageBuffer::Zeros(3, 2, 3, ImageDepth::uint8);
    uint8_t* row0 = img.ptr<uint8_t>(0);
    row0[0] = 10; row0[1] = 20; row0[2] = 30;   // pixel (0,0)
    row0[3] = 40; row0[4] = 50; row0[5] = 60;   // pixel (1,0)

    std::string info = MatrixInfoUtils::MatPixelColorInfo(img, 0, 0);
    CHECK(!info.empty());
    // Should contain "10" and "20" and "30"
    CHECK(info.find("10") != std::string::npos);
    CHECK(info.find("20") != std::string::npos);
    CHECK(info.find("30") != std::string::npos);

    // Out of bounds should return empty
    std::string oob = MatrixInfoUtils::MatPixelColorInfo(img, 10, 10);
    CHECK(oob.empty());

    PASS();
}

void test_mat_info()
{
    TEST("MatrixInfoUtils::_MatInfo");
    ImageBuffer img = ImageBuffer::Zeros(100, 50, 1, ImageDepth::float32);
    std::string info = MatrixInfoUtils::_MatInfo(img);
    CHECK(info.find("float32") != std::string::npos);
    CHECK(info.find("100") != std::string::npos);
    CHECK(info.find("50") != std::string::npos);

    PASS();
}


// =========================================================================
// ZoomPanTransform utility tests
// =========================================================================

void test_zoom_pan_visible_roi()
{
    TEST("ZoomPanTransform::VisibleRoi");
    // Identity zoom on a 100x100 image displayed at 100x100
    Matrix33d identity = Matrix33d::eye();
    Rect roi = ZoomPanTransform::VisibleRoi(identity, Size(100, 100), Size(100, 100));
    CHECK(roi.x == 0 && roi.y == 0);
    CHECK(roi.width == 100 && roi.height == 100);

    // 2x zoom centered at origin: visible area should be ~50x50
    Matrix33d zoom2x = Matrix33d::eye();
    zoom2x(0, 0) = 2.0;
    zoom2x(1, 1) = 2.0;
    Rect roi2 = ZoomPanTransform::VisibleRoi(zoom2x, Size(100, 100), Size(100, 100));
    CHECK(roi2.width <= 51 && roi2.width >= 49);
    CHECK(roi2.height <= 51 && roi2.height >= 49);

    PASS();
}

void test_make_full_view()
{
    TEST("ZoomPanTransform::MakeFullView");
    // Image wider than viewport
    Matrix33d m = ZoomPanTransform::MakeFullView(Size(200, 100), Size(100, 100));
    double zoom = m(0, 0);
    CHECK_NEAR(zoom, 0.5, 1e-6);  // must scale down by 2x to fit width

    // Image taller than viewport
    Matrix33d m2 = ZoomPanTransform::MakeFullView(Size(100, 200), Size(100, 100));
    double zoom2 = m2(0, 0);
    CHECK_NEAR(zoom2, 0.5, 1e-6);  // must scale down by 2x to fit height

    PASS();
}


// =========================================================================
// Main
// =========================================================================

int main()
{
    printf("=== ImmVision Internals Unit Tests ===\n\n");

    printf("[ImageBuffer]\n");
    test_image_buffer_zeros();
    test_image_buffer_clone();
    test_image_buffer_ptr();
    test_image_buffer_sub_image();
    test_image_buffer_copy_to();
    test_image_buffer_fill();

    printf("\n[Types]\n");
    test_color4d();
    test_rect();
    test_point2d_arithmetic();
    test_matrix33d_apply();
    test_image_depth_helpers();

    printf("\n[converted_to_rgba]\n");
    test_converted_to_rgba_uint8_3ch();
    test_converted_to_rgba_uint8_3ch_bgr();
    test_converted_to_rgba_uint8_1ch();
    test_converted_to_rgba_uint8_4ch();
    test_converted_to_rgba_float32_1ch();
    test_converted_to_rgba_int16_1ch();

    printf("\n[Colormap]\n");
    test_can_colormap();
    test_apply_colormap();

    printf("\n[MatrixInfoUtils]\n");
    test_mat_pixel_color_info();
    test_mat_info();

    printf("\n[ZoomPanTransform]\n");
    test_zoom_pan_visible_roi();
    test_make_full_view();

    printf("\n=== %d / %d tests passed ===\n", sPassCount, sTestCount);
    return (sPassCount == sTestCount) ? 0 : 1;
}
