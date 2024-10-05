#pragma once
#include "imgui.h"
#include <opencv2/core.hpp>


namespace ImmVision
{
    // GlTexture contains an OpenGL texture which can be created or updated from a cv::Mat (C++), or numpy array (Python)
    struct GlTexture
    {
        //
        // Constructors
        //

        // Create an empty texture
        GlTexture();
        // Create a texture from an image (cv::Mat in C++, numpy array in Python)
        // isColorOrderBGR: if true, the image is assumed to be in BGR order (OpenCV default)
        GlTexture(const cv::Mat& image, bool isColorOrderBGR = false);
        // The destructor will delete the texture from the GPU
        ~GlTexture();

        // GlTextureCv is non copiable (since it holds a reference to a texture stored on the GPU),
        // but it is movable.
        GlTexture(const GlTexture& ) = delete;
        GlTexture& operator=(const GlTexture& ) = delete;
        GlTexture(GlTexture&& other) noexcept = default;
        GlTexture& operator=(GlTexture&& other) noexcept = default;


        //
        // Methods
        //

        // Update the texture from a new image (cv::Mat in C++, numpy array in Python).
        void UpdateFromImage(const cv::Mat& image, bool isColorOrderBGR = false);
        // Returns the size as ImVec2
        ImVec2 SizeImVec2() const;


        //
        // Members
        //

        // OpenGL texture ID on the GPU
        ImTextureID TextureId;
        // Image size in pixels
        cv::Size Size;
    };
} // namespace ImmVision
