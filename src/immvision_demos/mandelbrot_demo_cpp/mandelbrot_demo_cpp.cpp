// close copy of pybind/pybind_examples/mandelbrot.py
#include "hello_imgui/hello_imgui.h"
#include "immvision/immvision.h"
#include "immvision/imgui_imm.h"
#ifndef __EMSCRIPTEN__
#include "fplus/fplus.hpp"
#endif

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <array>
#include <vector>
#include <optional>

//#define PRECISE_FLOAT_DOUBLE
#define PRECISE_FLOAT_LONG_DOUBLE

#if defined(PRECISE_FLOAT_DOUBLE)
using PreciseFloat = double;
#elif defined(PRECISE_FLOAT_LONG_DOUBLE)
using PreciseFloat = long double;
#endif

using PreciseCoords = std::array<PreciseFloat, 2>;
using ColorType = float;


PreciseFloat lerp(PreciseFloat a, PreciseFloat b, PreciseFloat x)
{
    return a + (b - a) * x;
}


cv::Mat mandelbrot(
    int width,
    int height,
    PreciseFloat x_center,
    PreciseFloat y_center,
    PreciseFloat zoom,
    int max_iterations
    )
{
    cv::Mat_<ColorType> result(cv::Size(width, height));

    // Compute boundings
    PreciseFloat x_width = PreciseFloat(1.5);
    PreciseFloat y_height = PreciseFloat(1.5) * height / width;
    PreciseFloat x_from = x_center - x_width / zoom;
    PreciseFloat x_to = x_center + x_width / zoom;
    PreciseFloat y_from = y_center - y_height / zoom;
    PreciseFloat y_to = y_center + y_height / zoom;

    // # parallel loop? (speedup by a factor of 7 on an 8 cores machines)
    auto fn_y_loop = [&](int iy)
    {
        PreciseFloat ky = PreciseFloat(iy) / PreciseFloat(height);
        PreciseFloat y0 = lerp(y_from, y_to, ky);

        for (int ix = 0; ix < width; ++ix)
        {
            PreciseFloat kx = PreciseFloat(ix) / PreciseFloat(width);

            // start iteration
            PreciseFloat x0 = lerp(x_from, x_to, kx);

            PreciseFloat x = PreciseFloat(0.0);
            PreciseFloat y = PreciseFloat(0.0);

            // perform Mandelbrot set iterations
            bool escaped = false;
            ColorType color;
            for (int iteration = 0; iteration < max_iterations; ++iteration)
            {
                PreciseFloat x_new = x * x - y * y + x0;
                y = 2 * x * y + y0;
                x = x_new;


                // if escaped
                PreciseFloat norm2 = x * x + y * y;
                if (norm2 > 4.0)
                {
                    escaped = true;
                    // color using pretty linear gradient
                    color = ColorType(1.0) - ColorType(0.01) * (ColorType(iteration) - log(log(ColorType(norm2))));
                    break;
                }
            }

            if (!escaped)
                color = ColorType(0.0);

            result(iy, ix) = color;

        }
        return true;
    };

#ifndef __EMSCRIPTEN__
#define PARALLEL_MANDEL
#endif

#ifdef PARALLEL_MANDEL
    auto y_range = fplus::numbers(0, height);
    fplus::transform_parallelly_n_threads(16, fn_y_loop, y_range);
#else
    for (int iy = 0; iy < height; ++iy)
        fn_y_loop(iy);
#endif

    return std::move(result);
}


struct MandelbrotParams
{
    MandelbrotParams & self;
    int width = 1000;
    int height = 750;
    // x in [-2, 1] and y in [-1, 1]
    PreciseFloat x_center = PreciseFloat(-0.5);
    PreciseFloat y_center = PreciseFloat(0.);
    PreciseFloat zoom = PreciseFloat(1.);
    int max_iterations = 100;

    MandelbrotParams() : self(*this) {}

    cv::Mat mandelbrot_image()
    {
        return mandelbrot(self.width, self.height, self.x_center, self.y_center, self.zoom, self.max_iterations);
    }

    void set_center(PreciseFloat x, PreciseFloat y)
    {
        self.x_center = x;
        self.y_center = y;
    }

    PreciseCoords get_center()
    {
        return {self.x_center, self.y_center};
    }

    PreciseCoords pixel_to_complex_coords(cv::Point2d pt)
    {
        PreciseFloat x_width = PreciseFloat(1.5);
        PreciseFloat y_height = PreciseFloat(1.5) * PreciseFloat(self.height) / PreciseFloat(self.width);
        PreciseFloat x_from = self.x_center - x_width / PreciseFloat(self.zoom);
        PreciseFloat x_to = self.x_center + x_width / PreciseFloat(self.zoom);
        PreciseFloat y_from = self.y_center - y_height / PreciseFloat(self.zoom);
        PreciseFloat y_to = self.y_center + y_height / PreciseFloat(self.zoom);

        PreciseFloat kx = PreciseFloat(pt.x) / PreciseFloat(self.width);
        PreciseFloat ky = PreciseFloat(pt.y) / PreciseFloat(self.height);
        PreciseFloat real = lerp(x_from, x_to, kx);
        PreciseFloat imag = lerp(y_from, y_to, ky);
        return {real, imag};
    }
};


// cf https://mandelbrot.dev/
std::vector<PreciseCoords >
    mandelbrot_poi_list = {
    {-0.5, 0.},
    {0.28439, - 0.01359},
    {0.28443, - 0.01273},
    {-0.79619, - 0.18323},
    {-0.748986, + 0.055768},
    {-0.658448, - 0.466852},
    {-0.715181, - 0.230028},
    {0.148865892, + 0.642407724},
    {-0.596360941, + 0.662749640},
    {-1.99991175020, + 0},
    {-1.67440967428, + 0.00004716557},
    {-0.139975337339, - 0.992076239092},
    {0.432456684114, - 0.340532264460},
};



class MandelbrotApp
{
    MandelbrotApp &self;
    MandelbrotParams mandelbrot_params  = MandelbrotParams();
    int idx_poi = 0;
    cv::Mat image;
    std::optional<PreciseCoords> last_mouse_coords;
    ImmVision::ImageParams image_params;

public:
    MandelbrotApp() : self(*this)
    {
        self.move_to_poi();
        self.update_image();

        self.image_params.ZoomWithMouseWheel = false;
        self.image_params.PanWithMouse = false;
        self.image_params.AddWatchedPixelOnDoubleClick = false;
    }

    void move_to_poi()
    {
        self.mandelbrot_params.x_center = mandelbrot_poi_list[idx_poi][0];
        self.mandelbrot_params.y_center = mandelbrot_poi_list[idx_poi][1];
    }

    void update_image()
    {
        self.image = self.mandelbrot_params.mandelbrot_image();
    }

    void gui()
    {
        auto& params = self.mandelbrot_params;
        bool needs_refresh = false;

        // Show FPS
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

        // On double-click, change the zooming center
        if (ImGui::IsMouseDoubleClicked(0) && self.last_mouse_coords.has_value())
        {
            auto mouse_coords = *self.last_mouse_coords;
            self.mandelbrot_params.set_center(mouse_coords[0], mouse_coords[1]);
            needs_refresh = true;
        }
        if (ImGui::IsMouseDragging(0) && self.last_mouse_coords.has_value())
        {
            ImVec2 delta = ImGui::GetMouseDragDelta(0);
            {
                cv::Point2d imageCenterWithDelta = {(double)self.mandelbrot_params.width / 2., (double)self.mandelbrot_params.height / 2.};
                imageCenterWithDelta.x -= double(delta.x);
                imageCenterWithDelta.y -= double(delta.y);
                PreciseCoords new_center_coords = params.pixel_to_complex_coords(imageCenterWithDelta);
                params.set_center(new_center_coords[0], new_center_coords[1]);
                needs_refresh = true;
                ImGui::ResetMouseDragDelta(0);
            }
        }
        if ((fabs(ImGui::GetIO().MouseWheel) > 0.f) && self.last_mouse_coords.has_value())
        {
            double zoomRatio = (double)ImGui::GetIO().MouseWheel / 8.;
            params.zoom = params.zoom * PreciseFloat(exp(zoomRatio));
            needs_refresh = true;
        }


        // Change location to interesting location
        ImGui::SetNextItemWidth(200.f);
        bool changed = ImGui::SliderInt(
            "interesting location", &self.idx_poi, 0, mandelbrot_poi_list.size() - 1);
        if (changed)
        {
            needs_refresh = true;
            self.move_to_poi();
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200.f);
        // Display center
        auto center = self.mandelbrot_params.get_center();
        ImGui::Text("%8LG + %8LG * i", (long double)center[0], (long double)center[1]);

        // Edit Zoom
        ImGui::SetNextItemWidth(250.f);
        PreciseFloat logZoom = log10(params.zoom);
        changed = ImGuiImm::SliderAnyFloat("log zoom", &logZoom, PreciseFloat(0.), PreciseFloat(20.));
        if (changed)
        {
            needs_refresh = true;
            params.zoom = pow(PreciseFloat(10.), logZoom);
        }
        ImGui::SameLine();
        ImGui::Text("Zoom = %8LG", (long double)params.zoom);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(250.f);
        // Edit max_iterations
        needs_refresh |= ImGui::SliderInt("iterations", &params.max_iterations, 1, 1000);

        // Edit image size
        ImGui::SetNextItemWidth(200.f);
        needs_refresh |=ImGui::SliderInt("Width", &params.width, 400, 2000);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200.f);
        needs_refresh |=ImGui::SliderInt("Height", &params.height, 400, 2000);

        // Edit position
        needs_refresh |= ImGuiImm::SliderAnyFloat("x", &params.x_center, PreciseFloat(-2.), PreciseFloat(1.));
        needs_refresh |= ImGuiImm::SliderAnyFloat("y", &params.y_center, PreciseFloat(-1.), PreciseFloat(1.));

        // recalculate image if needed
        if (needs_refresh)
            self.update_image();
        self.image_params.RefreshImage = needs_refresh;

        ImmVision::Image("mandelbrot", self.image, &self.image_params);
        cv::Point2d mouse_position = self.image_params.MouseInfo.MousePosition;
        if (mouse_position.x > 0.)
        {
            auto complex_coords = self.mandelbrot_params.pixel_to_complex_coords(mouse_position);
            ImGui::Text("Complex position: %8LG + %8LG * i", (long double)complex_coords[0], (long double)complex_coords[1]);
            self.last_mouse_coords = complex_coords;
        }
        else
            self.last_mouse_coords = std::nullopt;


    }

    void run()
    {
        HelloImGui::RunnerParams params;
        params.appWindowParams.windowSize = {1400, 1200};
        params.callbacks.ShowGui = [*this]() { self.gui(); };
        HelloImGui::Run(params);
    }


};


void test_mandel()
{
    auto params = MandelbrotParams();
    auto mandel_image = params.mandelbrot_image();
    cv::imshow("mandel_image", mandel_image);
    cv::waitKey();
}


void playground()
{
    auto app = MandelbrotApp();
    app.run();
}


void measure_perf()
{
#ifndef __EMSCRIPTEN__
    auto params = MandelbrotParams();

    fplus::benchmark_session benchmark_sess;
    auto make_mandelbrot = [&params] { auto m = params.mandelbrot_image(); };
    auto make_mandelbrot_bench = fplus::make_benchmark_void_function(benchmark_sess, "make_mandelbrot", make_mandelbrot);

    for (int i = 0; i < 100; ++i)
        make_mandelbrot_bench();

    std::cout << benchmark_sess.report();
#endif
}


int main(int, char* [])
{
    //test_mandel();
    //measure_perf();
    playground();

    return 0;
}

