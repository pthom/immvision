/* 
TODO

 - smooth_colors : smooth color
 
- x_center / y_center / zoom in Params -> LocInfo
  smooth_colors

*/

// close copy of pybind/pybind_examples/mandelbrot.py
#include "hello_imgui/hello_imgui.h"
#include "immvision/immvision.h"
#include "immvision/imgui_imm.h"
#ifndef __EMSCRIPTEN__
#include "nlohmann_json.hpp"
#endif

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <array>
#include <vector>
#include <optional>
#include <string>
#include <fstream>

#define PRECISE_FLOAT_DOUBLE
//#define PRECISE_FLOAT_LONG_DOUBLE

#if defined(PRECISE_FLOAT_DOUBLE)
using PreciseFloat = double;
#elif defined(PRECISE_FLOAT_LONG_DOUBLE)
using PreciseFloat = long double;
#endif

using PreciseCoords = std::array<PreciseFloat, 2>;
using ColorType = double;
//using ColorType = int;


std::string ASSETS_FOLDER = "assets/";
std::string LOCATIONS_FILE = ASSETS_FOLDER + "/locations.json";


bool InputText_Str(const char* label, std::string* str, ImGuiInputTextFlags flags = 0)
{
    return ImGui::InputText(label, (char*)str->c_str(), str->capacity() + 1, flags);
}

std::string precise_coords_to_string(PreciseCoords c)
{
    typedef std::numeric_limits< long double > limit;
    auto max_digits = limit::max_digits10;
    std::stringstream ss;
    ss.precision(max_digits);
    ss << c[0] << ", " << c[1];
    std::string r = ss.str();
    return r;
}

PreciseFloat precise_lerp(PreciseFloat a, PreciseFloat b, PreciseFloat x)
{
    return a + (b - a) * x;
}


cv::Mat_<ColorType> mandelbrot(
    int width,
    int height,
    PreciseFloat x_center,
    PreciseFloat y_center,
    PreciseFloat zoom,
    int max_iterations,
    bool smooth_colors
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
        PreciseFloat y0 = precise_lerp(y_from, y_to, ky);

        for (int ix = 0; ix < width; ++ix)
        {
            PreciseFloat kx = PreciseFloat(ix) / PreciseFloat(width);

            // start iteration
            PreciseFloat x0 = precise_lerp(x_from, x_to, kx);

            PreciseFloat x = PreciseFloat(0.0);
            PreciseFloat y = PreciseFloat(0.0);

            // perform Mandelbrot set iterations
            bool escaped = false;
            ColorType color = ColorType();
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
                    if (smooth_colors)
                        // color using pretty linear gradient
                        color = ColorType(iteration) - log(log(ColorType(norm2))); //* 100.;
                    else
                        color = ColorType(iteration);
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
//#define PARALLEL_MANDEL
#endif

#ifdef PARALLEL_MANDEL
    auto y_range = fplus::numbers(0, height);
    fplus::transform_parallelly_n_threads(128, fn_y_loop, y_range);
#else
    for (int iy = 0; iy < height; ++iy)
        fn_y_loop(iy);
#endif

    return result;
}


struct MandelbrotParams
{
    PreciseCoords coords;
    std::string name;
    std::string image_file = "";
    std::string colormap_name = "None";

    PreciseFloat zoom = PreciseFloat(1.);
    int max_iterations = 1400;
    bool automatic_iterations_increase = true;
    bool smooth_colors = false;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        MandelbrotParams,
        coords,
        name,
        image_file,
        colormap_name,
        zoom,
        max_iterations,
        automatic_iterations_increase,
        smooth_colors
        );

    void gui_location(std::optional<PreciseCoords> mouse_coords)
    {
        auto& self = *this;
        // Display center
        ImGui::Text("Image center");
        std::string currentCenterStr= precise_coords_to_string(self.get_center());
        ImGui::Text("%s", currentCenterStr.c_str());
        ImGui::SameLine();
        if (ImGui::Button("Copy to clipboard"))
        {
            ImGui::SetClipboardText(currentCenterStr.c_str());
        }

        ImGui::Text("Mouse Location");
        if (mouse_coords.has_value())
            ImGui::Text("%s", precise_coords_to_string(*mouse_coords).c_str());
        else
            ImGui::NewLine();
    }

    bool gui_options()
    {
        auto& self = *this;

        bool needs_refresh = false;
        needs_refresh |= ImGui::Checkbox("Smooth colors", &self.smooth_colors);

        // Edit max_iterations
        ImGui::Text("Nb iterations");
        ImGui::Checkbox("Increase with zoom", &self.automatic_iterations_increase);
        ImGui::SetNextItemWidth(200.f);
        std::string label_iterations = self.automatic_iterations_increase ? "Max iterations" : "Nb iterations";
        needs_refresh |= ImGui::SliderInt(label_iterations.c_str(), &self.max_iterations, 1, 5000);

        return needs_refresh;
    }

    bool gui_zoom()
    {
        auto& self = *this;
        bool needs_refresh = false;

        ImGui::Text("Zoom");
        ImGui::Text("Zoom = %8LG", (long double)self.zoom);

        ImGui::PushButtonRepeat(true);
        if (ImGui::Button("Zoom -")) {
            self.zoom /= 1.2;
            needs_refresh = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Zoom +"))
        {
            self.zoom *= 1.2;
            needs_refresh = true;
        }
        ImGui::PopButtonRepeat();

        return needs_refresh;
    }

    bool gui_params(std::optional<PreciseCoords> mouse_coords)
    {
        auto& self = *this;
        bool needs_refresh = false;

        if (ImGui::BeginTabBar("params_gui"))
        {
            if (ImGui::BeginTabItem("Zoom"))
            {
                needs_refresh |= self.gui_zoom();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Options"))
            {
                needs_refresh |= self.gui_options();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Location"))
            {
                gui_location(mouse_coords);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        return needs_refresh;
    }

    enum class GuiListAction
    {
        Go,
        Go_Animate,
        Delete,
        None
    };

    GuiListAction gui_in_list() const
    {
        ImGui::PushID(this);

        auto& self = *this;

        GuiListAction action = GuiListAction::None;

        float s = 70.f;
        if (!self.image_file.empty())
            HelloImGui::ImageFromAsset(self.image_file.c_str(), ImVec2(s, s));
        else
            ImGui::Dummy(ImVec2(s, s));
        ImGui::SameLine();

        ImGui::BeginGroup();
        if (ImGui::Button("Go"))
            action = GuiListAction::Go;
        ImGui::SameLine();
        if (ImGui::Button("Animate zoom"))
            action = GuiListAction::Go_Animate;

        ImGui::Text("%s", self.name.c_str());

        if (ImGui::Button("Delete"))
            ImGui::OpenPopup("Delete?");
        if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Are you sure?\nThis operation cannot be undone!\n\n");
            ImGui::Separator();

            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                action = GuiListAction::Delete;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::EndGroup();

        ImGui::PopID();

        return action;
    }

    void set_center(PreciseCoords c)
    {
        auto& self = *this;
        self.coords = c;
    }

    PreciseCoords get_center()
    {
        auto self = *this;
        return self.coords;
    }

    int nb_iterations()
    {
        auto& self = *this;
        int real_nb_iterations = self.max_iterations;
        if (self.automatic_iterations_increase)
        {
            // log zoom = 0 -> 300 it
            // log zoom = 8 -> 1400
            // log zoom = 13 -> 2000
            double it_at_0 = 300.;
            double it_at_13 = 2000;
            PreciseFloat logZoom = log10(self.zoom);
            double it = it_at_0 + logZoom * (it_at_13 - it_at_0) / 13.;
            real_nb_iterations = (int)it;
        }
        return real_nb_iterations;
    }

    cv::Mat mandelbrot_image(cv::Size image_size)
    {
        auto& self = *this;
        return mandelbrot(
            image_size.width, image_size.height,
            self.coords[0], self.coords[1],
            self.zoom,
            self.nb_iterations(),
            self.smooth_colors);
    }

    PreciseCoords pixel_to_complex_coords(cv::Point2d pt, cv::Size image_size)
    {
        auto& self = *this;

        PreciseFloat x_center = self.coords[0];
        PreciseFloat y_center = self.coords[1];

        PreciseFloat x_width = PreciseFloat(1.5);
        PreciseFloat y_height = PreciseFloat(1.5) * PreciseFloat(image_size.height) / PreciseFloat(image_size.width);
        PreciseFloat x_from = x_center - x_width / PreciseFloat(self.zoom);
        PreciseFloat x_to = x_center + x_width / PreciseFloat(self.zoom);
        PreciseFloat y_from = y_center - y_height / PreciseFloat(self.zoom);
        PreciseFloat y_to = y_center + y_height / PreciseFloat(self.zoom);

        PreciseFloat kx = PreciseFloat(pt.x) / PreciseFloat(image_size.width);
        PreciseFloat ky = PreciseFloat(pt.y) / PreciseFloat(image_size.height);
        PreciseFloat real = precise_lerp(x_from, x_to, kx);
        PreciseFloat imag = precise_lerp(y_from, y_to, ky);
        return {real, imag};
    }
};


// cf https://mandelbrot.dev/
std::vector<MandelbrotParams> default_mandelbrot_params_locations()
{
    return {
        {{-0.5, 0.}, "Au centre, rien à voir", "centre.jpg"},
        {{0.2842921050702919, -0.013582736321369538}, "Des fougères ?", "fougeres.jpg"},
        {{0.28443003463171357, -0.012730088053867291},"Des singes (it=1200, zoom=5E10)", "singes.jpg"},
        {{-0.79610739007462483, -0.18324251614028589},"Des fleurs ?", "fleurs.jpg"},
        {{-0.74898667736294577, 0.055767770785011703},"Pieuvres", "pieuvres.jpg"},
        {{-0.65844589596458547, -0.46685022919462682},"minimandels et feux d'artifice", "mandelfeux.jpg"},
        {{-0.71518148532829584, -0.23002428075714421},"Dragons", "dragons.jpg"},
        {{-0.71518148532829584, -0.23002428075714421},"C'est l'été", "ete.jpg"},
        {{-0.596360946896342, 0.66274963725780922},"Miro", "miro.jpg"},
        {{-1.9999117503937673, -3.4259736701971334e-10},"Psychédélique 1970", "psyche.jpg"},
        {{-1.6744096735768377, 4.7165750741621498e-05},"Art abstrait", "abstrait.jpg"},
        {{-0.14020508000698131, -0.99225545054791242},"des vaisseaux sanguins ?", "vaisseaux.jpg"},
        {{0.4324566841138926, -0.34053226446303003},"Feux d'artifices puis minimandels", "feuxmandel.jpg"},
    };
}


struct MandelbrotParamsList
{
    std::vector<MandelbrotParams> params_list;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        MandelbrotParamsList,
        params_list);

    MandelbrotParamsList()
    {
    }

    static MandelbrotParamsList saved_list()
    {
        std::ifstream f(LOCATIONS_FILE);
        nlohmann::json j;
        f >> j;

        MandelbrotParamsList r = j.get<MandelbrotParamsList>();
        return r;
    }

    void save_list()
    {
        auto& self = *this;
        nlohmann::json j = self;
        std::ofstream o(LOCATIONS_FILE);
        o << std::setw(4) << j << std::endl;
    }

    void gui_save(MandelbrotParams& currentParam)
    {
        auto& self = *this;

        ImGui::SetNextItemWidth(200.f);
        InputText_Str("Name", &currentParam.name);
        ImGui::SetNextItemWidth(200.f);
        InputText_Str("Image file", &currentParam.image_file);

        if (ImGui::Button("Save"))
        {
            cv::Mat image_rgba =  ImmVision::GetCachedRgbaImage("mandelbrot");
            cv::Mat image_bgr;
            cv::cvtColor(image_rgba, image_bgr, cv::COLOR_RGBA2BGR);
            cv::imwrite(ASSETS_FOLDER + currentParam.image_file, image_bgr);

            self.params_list.push_back(currentParam);
            self.save_list();
        }
    }

    struct GuiListAction
    {
        std::optional<MandelbrotParams> new_param;
        bool animate = false;
    };
    GuiListAction gui_list(MandelbrotParams& currentParam)
    {
        auto& self = *this;

        GuiListAction gui_list_action;
        // Change location to interesting location
        ImGui::Text("Saved Locations");
        ImGui::SetNextItemWidth(200.f);
        ImGui::BeginChild("POIS", ImVec2(300.f, 750.f));

        MandelbrotParams const * to_delete = nullptr;
        for (const auto &param: self.params_list)
        {
            auto param_action = param.gui_in_list();
            if (param_action == MandelbrotParams::GuiListAction::Go)
                gui_list_action.new_param = param;
            else if(param_action == MandelbrotParams::GuiListAction::Go_Animate)
            {
                gui_list_action.new_param = param;
                gui_list_action.animate = true;
            }
            else if (param_action == MandelbrotParams::GuiListAction::Delete)
                to_delete = &param;
        }

        if (to_delete != nullptr)
        {
            std::vector<MandelbrotParams> new_list;
            for (const auto& param: self.params_list)
                if (&param != to_delete)
                    new_list.push_back(param);
            self.params_list = new_list;

            self.save_list();
        }

        ImGui::EndChild();

        gui_save(currentParam);

        return gui_list_action;
    }
};


cv::Size cv_size_factor(cv::Size s, double factor)
{
    cv::Size r(
        (int)((double)s.width * factor),
        (int)((double)s.height * factor)
        );
    return r;
}


class MandelbrotApp
{
    MandelbrotParamsList mandelbrot_params_list;

    MandelbrotParams mandelbrot_params  = MandelbrotParams();

    PreciseFloat animate_final_zoom = 1.f;
    bool         is_animating = false;

    double animation_image_reduce_factor = 2.;
    cv::Size final_image_size = {800, 800};

    cv::Mat image;
    ImmVision::ImageParams image_params;

public:
    MandelbrotApp()
    {
        auto& self = *this;
        self.mandelbrot_params_list = MandelbrotParamsList::saved_list();
        self.mandelbrot_params = self.mandelbrot_params_list.params_list.front();
        self.update_image();

        self.image_params.ZoomWithMouseWheel = false;
        self.image_params.PanWithMouse = false;
        self.image_params.AddWatchedPixelOnDoubleClick = false;
        self.image_params.ColormapSettings.Colormap = "Jet";
        self.image_params.ShowImageInfo = false;
        self.image_params.ShowZoomButtons = false;
        self.image_params.ShowOptionsPanel = true;
    }

    cv::Size current_image_size()
    {
        auto& self = *this;
        if (self.is_animating)
            return cv_size_factor(self.final_image_size, 1. / self.animation_image_reduce_factor);
        else
            return self.final_image_size;
    }

    void update_image()
    {
        auto& self = *this;
        self.image = self.mandelbrot_params.mandelbrot_image(self.current_image_size());
    }

    std::optional<PreciseCoords> mouse_coords()
    {
        auto& self = *this;
        auto mouseImageCoords = self.image_params.MouseInfo.MousePosition;
        if (mouseImageCoords.x > 0)
            return  self.mandelbrot_params.pixel_to_complex_coords(mouseImageCoords, self.current_image_size());
        else
            return std::nullopt;
    }

    bool is_mouse_hovering_image()
    {
        auto& self = *this;
        auto mouseImageCoords = self.image_params.MouseInfo.MousePosition;
        return mouseImageCoords.x >= 0.;
    }

    bool handle_mouse()
    {
        auto& self = *this;
        auto& params = self.mandelbrot_params;
        bool needs_refresh = false;

        // On double-click, change the zooming center
        if (ImGui::IsMouseDoubleClicked(0) && self.mouse_coords().has_value())
        {
            self.mandelbrot_params.set_center(self.mouse_coords().value());
            needs_refresh = true;
            self.is_animating = false;
        }


        if (ImGui::IsMouseDragging(0) && self.is_mouse_hovering_image())
        {
            ImVec2 delta = ImGui::GetMouseDragDelta(0);
            {
                cv::Size image_size = self.current_image_size();
                cv::Point2d imageCenterWithDelta = {(double)image_size.width / 2., (double)image_size.height / 2.};
                imageCenterWithDelta.x -= double(delta.x);
                imageCenterWithDelta.y -= double(delta.y);
                PreciseCoords new_center_coords = params.pixel_to_complex_coords(imageCenterWithDelta, image_size);
                params.set_center(new_center_coords);
                needs_refresh = true;
                ImGui::ResetMouseDragDelta(0);
                self.is_animating = false;
            }
        }
        if ((fabs(ImGui::GetIO().MouseWheel) > 0.f) && self.is_mouse_hovering_image())
        {
            double zoomRatio = (double)ImGui::GetIO().MouseWheel / 8.;
            params.zoom = params.zoom * PreciseFloat(exp(zoomRatio));
            needs_refresh = true;
            self.is_animating = false;
        }

        return needs_refresh;
    }

    bool _do_animate()
    {
        auto &self = *this;
        if (!self.is_animating)
            return false;

        self.mandelbrot_params.zoom *= 1.1;

        if (self.mandelbrot_params.zoom >= self.animate_final_zoom)
            self.is_animating = false;

        return true;
    }

    bool gui_image_size()
    {
        auto& self = *this;
        bool needs_refresh = false;
        ImGui::Text("Image Size");
        ImGui::SetNextItemWidth(200.f);
        needs_refresh |=ImGui::SliderInt("Width", &self.final_image_size.width, 400, 2000);
        ImGui::SetNextItemWidth(200.f);
        needs_refresh |=ImGui::SliderInt("Height", &self.final_image_size.height, 400, 2000);
        return needs_refresh;
    }

    void gui()
    {
        auto& self = *this;
        bool needs_refresh = false;

        if (handle_mouse())
            needs_refresh = true;

        if (_do_animate())
            needs_refresh = true;

        ImGui::BeginGroup();

        // Show FPS
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

        needs_refresh |= self.gui_image_size();

        // Params GUI
        needs_refresh |= self.mandelbrot_params.gui_params(self.mouse_coords());

        // Locations list
        auto gui_list_action = self.mandelbrot_params_list.gui_list(self.mandelbrot_params);
        if (gui_list_action.new_param.has_value())
        {
            self.mandelbrot_params = *gui_list_action.new_param;
            self.image_params.ColormapSettings.Colormap = self.mandelbrot_params.colormap_name;

            if (gui_list_action.animate)
            {
                self.is_animating = true;
                self.animate_final_zoom = gui_list_action.new_param->zoom;
                self.mandelbrot_params.zoom = 1.;
            }

            needs_refresh = true;
        }

        // recalculate image if needed
        if (needs_refresh)
            self.update_image();

        self.image_params.RefreshImage = needs_refresh;
        self.image_params.ImageDisplaySize = self.final_image_size;
        self.image_params.ZoomPanMatrix = ImmVision::MakeZoomPanMatrix_FullView(self.current_image_size(), self.final_image_size);

        ImGui::EndGroup();
        ImGui::SameLine();

        ImmVision::Image("mandelbrot", self.image, &self.image_params);

        self.mandelbrot_params.colormap_name = self.image_params.ColormapSettings.Colormap;
    }

    void run()
    {
        auto& self = *this;
        HelloImGui::RunnerParams params;
        params.appWindowParams.windowGeometry.size = {1600, 1200};
        params.callbacks.ShowGui = [&self]() { self.gui(); };
        HelloImGui::Run(params);
    }
};


void playground()
{
    auto app = MandelbrotApp();
    app.run();
}

//
//void measure_perf()
//{
//#ifndef __EMSCRIPTEN__
//    auto params = MandelbrotParams();
//
//    fplus::benchmark_session benchmark_sess;
//    auto make_mandelbrot = [&params] { auto m = params.mandelbrot_image({600, 600}); };
//    auto make_mandelbrot_bench = fplus::make_benchmark_void_function(benchmark_sess, "make_mandelbrot", make_mandelbrot);
//
//    for (int i = 0; i < 100; ++i)
//        make_mandelbrot_bench();
//
//    std::cout << benchmark_sess.report();
//#endif
//}


int main(int, char* [])
{
    //test_mandel();
    //measure_perf();
    playground();

    return 0;
}

