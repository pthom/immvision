#include "immvision/internal/imgui_imm_gl_image.h"

#include <string>

namespace ImGuiImmGlImage
{
//#define IMMVISION_BUILDING_PYBIND // KK
    static void* UintToTextureID(unsigned int id)
    {
        return (void *)(intptr_t)id;
    }

#ifndef IMMVISION_BUILDING_PYBIND
    void  Image(unsigned int user_texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
    {
        ImGui::Image(UintToTextureID(user_texture_id), size, uv0, uv1, tint_col, border_col);
    }
    bool  ImageButton(unsigned int user_texture_id, const ImVec2& size, const ImVec2& uv0,  const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
    {
        return ImGui::ImageButton(UintToTextureID(user_texture_id), size, uv0, uv1, frame_padding, bg_col, tint_col);
    }
    void  GetWindowDrawList_AddImage(unsigned int user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col)
    {
        ImGui::GetWindowDrawList()->AddImage(UintToTextureID(user_texture_id), p_min, p_max, uv_min, uv_max, col);
    }
#endif
} // namespace ImGuiImmGlImage
