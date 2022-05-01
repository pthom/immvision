#include "immvision/image.h"
#include "immvision/internal/misc/immvision_to_string.h"
#include "cvnp/cvnp.h"
#include "transfer_imgui_context.h"

#include <opencv2/core.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>


namespace py = pybind11;



void pydef_image(pybind11::module& m)
{
    using namespace ImmVision;

    // <autogen:pydef_struct> // Autogenerated code below! Do not edit!

    auto pyClassColorAdjustmentsValues = py::class_<ColorAdjustmentsValues>(m, "ColorAdjustmentsValues") 
        .def(py::init<>()) 
        .def_readwrite("factor", &ColorAdjustmentsValues::Factor, "Pre-multiply values by a Factor before displaying")
        .def_readwrite("delta", &ColorAdjustmentsValues::Delta, "Add a delta to the values before displaying")
        .def("__repr__", [](const ColorAdjustmentsValues& v) { return ToString(v); }); 

    auto pyClassMouseInformation = py::class_<MouseInformation>(m, "MouseInformation") 
        .def(py::init<>()) 
        .def_readwrite("mouse_position", &MouseInformation::MousePosition, "Mouse position in the original image/matrix\nThis position is given with float coordinates, and will be (-1., -1.) if the mouse is not hovering the image")
        .def_readwrite("mouse_position_displayed", &MouseInformation::MousePosition_Displayed, "Mouse position in the displayed portion of the image (the original image can be zoomed,\nand only show a subset if it may be shown).\nThis position is given with integer coordinates, and will be (-1, -1) if the mouse is not hovering the image")
        .def("__repr__", [](const MouseInformation& v) { return ToString(v); }); 

    auto pyClassImageParams = py::class_<ImageParams>(m, "ImageParams") 
        .def(py::init<>()) 
        .def_readwrite("refresh_image", &ImageParams::RefreshImage, "Refresh Image: images textures are cached. Set to true if your image matrix/buffer has changed\n(for example, for live video images)")
        .def_readwrite("image_display_size", &ImageParams::ImageDisplaySize, "Size of the displayed image (can be different from the matrix size)\nIf you specify only the width or height (e.g (300, 0), then the other dimension\nwill be calculated automatically, respecting the original image w/h ratio.")
        .def_readwrite("legend", &ImageParams::Legend, "Title displayed in the border")
        .def_readwrite("zoom_pan_matrix", &ImageParams::ZoomPanMatrix, "ZoomPanMatrix can be created using MakeZoomPanMatrix to create a view centered around a given point")
        .def_readwrite("zoom_key", &ImageParams::ZoomKey, "If displaying several images, those with the same ZoomKey will zoom and pan together")
        .def_readwrite("color_adjustments", &ImageParams::ColorAdjustments, "Color adjustments for float matrixes")
        .def_readwrite("color_adjustments_key", &ImageParams::ColorAdjustmentsKey, "If displaying several images, those with the same ColorAdjustmentsKey will adjust together")
        .def_readwrite("pan_with_mouse", &ImageParams::PanWithMouse, "")
        .def_readwrite("zoom_with_mouse_wheel", &ImageParams::ZoomWithMouseWheel, "")
        .def_readwrite("is_color_order_bgr", &ImageParams::IsColorOrderBGR, "Color Order: RGB or RGBA versus BGR or BGRA (Note: by default OpenCV uses BGR and BGRA)")
        .def_readwrite("selected_channel", &ImageParams::SelectedChannel, "if SelectedChannel >= 0 then only this channel is displayed")
        .def_readwrite("show_alpha_channel_checkerboard", &ImageParams::ShowAlphaChannelCheckerboard, "show a checkerboard behind transparent portions of 4 channels RGBA images")
        .def_readwrite("show_grid", &ImageParams::ShowGrid, "Grid displayed when the zoom is high")
        .def_readwrite("draw_values_on_zoomed_pixels", &ImageParams::DrawValuesOnZoomedPixels, "Pixel values show when the zoom is high")
        .def_readwrite("show_image_info", &ImageParams::ShowImageInfo, "Show matrix type and size")
        .def_readwrite("show_pixel_info", &ImageParams::ShowPixelInfo, "Show pixel values")
        .def_readwrite("show_zoom_buttons", &ImageParams::ShowZoomButtons, "Show buttons that enable to zoom in/out (the mouse wheel also zoom)")
        .def_readwrite("show_legend_border", &ImageParams::ShowLegendBorder, "Show a rectangular border with the legend")
        .def_readwrite("show_options", &ImageParams::ShowOptions, "Open the options panel")
        .def_readwrite("show_options_in_tooltip", &ImageParams::ShowOptionsInTooltip, "If set to true, then the option panel will be displayed in a transient tooltip window")
        .def_readwrite("watched_pixels", &ImageParams::WatchedPixels, "List of Watched Pixel coordinates")
        .def_readwrite("highlight_watched_pixels", &ImageParams::HighlightWatchedPixels, "Shall the watched pixels be drawn on the image")
        .def_readwrite("mouse_info", &ImageParams::MouseInfo, "Mouse position information. These values are filled after displaying an image")
        .def("__repr__", [](const ImageParams& v) { return ToString(v); }); 


        // </autogen:pydef_struct> // Autogenerated end


        // <autogen:pydef_function> // Autogenerated code below! Do not edit!

        m.def("factor_image_params_display_only", ImmVision::FactorImageParamsDisplayOnly, " Create ImageParams that display the image only, with no decoration, and no user interaction");
        m.def("make_zoom_pan_matrix", ImmVision::MakeZoomPanMatrix, " Create a zoom/pan matrix centered around a given point of interest");
        m.def("image", ImmVision::Image, " Display an image, with full user control: zoom, pan, watch pixels, etc.\n\n Notes:\n\n - the ImageParams may be modified by this function: you can extract from them\n   the mouse position, watched pixels, etc. Thus, their scope should extend beyond the call to Image !\n   If you cannot zoom/pan in a displayed image, extend the scope of the ImageParams!\n\n - This function requires that both imgui and OpenGL were initialized.\n   (for example, use `imgui_runner.run`for Python,  or `HelloImGui::Run` for C++)");
        m.def("image_display", ImmVision::ImageDisplay, " Only, display the image, with no decoration, and no user interaction\n\n Parameters:\n      - imageDisplaySize:\n          Size of the displayed image (can be different from the matrix size)\n          If you specify only the width or height (e.g (300, 0), then the other dimension\n          will be calculated automatically, respecting the original image w/h ratio.\n      - refreshImage: images textures are cached. Set to True if your image matrix/buffer has changed\n          (for example, for live video images)\n\n Note: this function requires that both imgui and OpenGL were initialized.\n       (for example, use `imgui_runner.run`for Python,  or `HelloImGui::Run` for C++)\n\n Todo: add a global for isBgrOrBgra");
        m.def("clear_texture_cache", ImmVision::ClearTextureCache, " Clears the internal texture cache of immvision (this is done automatically at exit time)\n\n Note: this function requires that both imgui and OpenGL were initialized.\n       (for example, use `imgui_runner.run`for Python,  or `HelloImGui::Run` for C++)");
        m.def("version_info", ImmVision::VersionInfo, " Return immvision version info");

        // </autogen:pydef_function> // Autogenerated code end

}