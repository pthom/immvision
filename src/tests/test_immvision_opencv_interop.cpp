// Unit tests for the header-only OpenCV conversions of ImmVision types.
// No GUI or OpenGL required.
//
// This target defines IMMVISION_HAS_OPENCV and links opencv_core by itself:
// the immvision library is built without any knowledge of OpenCV.

#include "immvision/immvision_types.h"

#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <vector>

using namespace ImmVision;

static int sTestCount = 0;
static int sPassCount = 0;

#define TEST(name) \
    do { sTestCount++; printf("  TEST: %s ... ", name); } while(0)

#define PASS() \
    do { sPassCount++; printf("OK\n"); } while(0)

#define CHECK(cond) \
    do { if (!(cond)) { printf("FAIL at %s:%d: %s\n", __FILE__, __LINE__, #cond); exit(1); } } while(0)


struct DepthPair { int cvDepth; ImageDepth depth; };
static const std::vector<DepthPair> sDepthPairs = {
    {CV_8U, ImageDepth::uint8}, {CV_8S, ImageDepth::int8},
    {CV_16U, ImageDepth::uint16}, {CV_16S, ImageDepth::int16},
    {CV_32S, ImageDepth::int32},
    {CV_32F, ImageDepth::float32}, {CV_64F, ImageDepth::float64},
};

// Takes an ImageBuffer, so that calling it with a cv::Mat exercises the implicit conversion
static size_t NbBytesPerRow(const ImageBuffer& b) { return b.step; }


static void test_mat_to_image_buffer_all_types()
{
    TEST("cv::Mat -> ImageBuffer -> cv::Mat, all depths and channels (zero-copy)");
    for (const auto& dp : sDepthPairs)
    {
        for (int ch : {1, 3, 4})
        {
            cv::Mat mat(5, 7, CV_MAKETYPE(dp.cvDepth, ch));
            ImageBuffer buf(mat);
            CHECK(buf.data == mat.data);
            CHECK(buf.width == 7 && buf.height == 5 && buf.channels == ch);
            CHECK(buf.depth == dp.depth);
            CHECK(buf.step == mat.step[0]);
            CHECK(NbBytesPerRow(mat) == mat.step[0]);

            cv::Mat back = buf.to_cv_mat();
            CHECK(back.data == mat.data);
            CHECK(back.type() == mat.type());
            CHECK(back.size() == mat.size());
            CHECK(back.step[0] == mat.step[0]);
        }
    }
    PASS();
}

static void test_non_continuous_roi()
{
    TEST("non continuous cv::Mat (ROI) is cloned");
    cv::Mat mat(10, 10, CV_8UC1);
    for (int y = 0; y < 10; y++)
        for (int x = 0; x < 10; x++)
            mat.at<uint8_t>(y, x) = (uint8_t)(y * 10 + x);
    cv::Mat roi = mat(cv::Rect(2, 3, 4, 5));
    CHECK(!roi.isContinuous());

    ImageBuffer buf(roi);
    CHECK(buf.data != roi.data);
    CHECK(buf.width == 4 && buf.height == 5);
    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 4; x++)
            CHECK(buf.ptr<uint8_t>(y)[x] == roi.at<uint8_t>(y, x));
    PASS();
}

static void test_lifetime()
{
    TEST("ImageBuffer keeps the cv::Mat data alive");
    ImageBuffer buf;
    {
        cv::Mat mat(4, 4, CV_32FC1, cv::Scalar(42.f));
        CHECK(mat.u->refcount == 1);
        buf = ImageBuffer(mat);
        CHECK(mat.u->refcount == 2);
    }
    CHECK(buf.ptr<float>(3)[3] == 42.f);

    // A clone made by the immvision library (which knows nothing about OpenCV) is independent
    ImageBuffer cloned = buf.clone();
    CHECK(cloned.data != buf.data);
    CHECK(cloned.ptr<float>(3)[3] == 42.f);
    PASS();
}

static void test_to_cv_mat_clone_and_empty()
{
    TEST("to_cv_mat_clone, empty buffer");
    ImageBuffer buf = ImageBuffer::Zeros(6, 3, 3, ImageDepth::uint16);
    cv::Mat view = buf.to_cv_mat();
    cv::Mat deep = buf.to_cv_mat_clone();
    CHECK(view.data == buf.data);
    CHECK(deep.data != buf.data);
    CHECK(deep.type() == CV_16UC3 && deep.cols == 6 && deep.rows == 3);

    CHECK(ImageBuffer().to_cv_mat().empty());
    PASS();
}

static void test_unsupported_depth()
{
    TEST("unsupported cv depth throws");
    bool didThrow = false;
    try { ImageBuffer buf(cv::Mat(2, 2, CV_16FC1)); }
    catch (const std::runtime_error&) { didThrow = true; }
    CHECK(didThrow);
    PASS();
}

static void test_small_types()
{
    TEST("Point, Point2d, Size, Rect, Color4d, Matrix33d");
    Point p = cv::Point(1, 2);
    CHECK(p == Point(1, 2));
    CHECK(cv::Point(p) == cv::Point(1, 2));

    Point2d p2 = cv::Point2d(1.5, 2.5);
    CHECK(p2 == Point2d(1.5, 2.5));
    CHECK(cv::Point2d(p2) == cv::Point2d(1.5, 2.5));

    Size s = cv::Size(3, 4);
    CHECK(s == Size(3, 4));
    CHECK(cv::Size(s) == cv::Size(3, 4));

    Rect r = cv::Rect(1, 2, 3, 4);
    CHECK(r.x == 1 && r.y == 2 && r.width == 3 && r.height == 4);
    CHECK(cv::Rect(r) == cv::Rect(1, 2, 3, 4));

    Color4d c = cv::Scalar(1., 2., 3., 4.);
    CHECK(c[0] == 1. && c[1] == 2. && c[2] == 3. && c[3] == 4.);
    CHECK(cv::Scalar(c) == cv::Scalar(1., 2., 3., 4.));

    cv::Matx33d cvMat(1, 2, 3, 4, 5, 6, 7, 8, 9);
    Matrix33d m = cvMat;
    CHECK(m(0, 1) == 2. && m(2, 0) == 7.);
    CHECK(cv::Matx33d(m) == cvMat);
    PASS();
}


int main()
{
    printf("=== ImmVision OpenCV interop tests ===\n");
    test_mat_to_image_buffer_all_types();
    test_non_continuous_roi();
    test_lifetime();
    test_to_cv_mat_clone_and_empty();
    test_unsupported_depth();
    test_small_types();

    printf("\n=== %d / %d tests passed ===\n", sPassCount, sTestCount);
    return (sPassCount == sTestCount) ? 0 : 1;
}
